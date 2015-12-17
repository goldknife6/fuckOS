#include <mm/slab.h>
#include <mm/mmu.h>
#include <mm/pages.h>
#include <mm/mmzone.h>

//#include <minios/lapic.h>
#include <fuckOS/kernel.h>

#include <string.h>
//__cacheline_aligned;

static struct mem_cache kmalloc_caches[PAGE_SHIFT];
static uint32_t cache_size[PAGE_SHIFT] = {1<<3,1<<4,1<<5,1<<6,96,1<<7,192,1<<8,1<<9,1<<10,1<<11,1<<12};
//8,16,32,64,96,128,192,256,512,1024,2048,4096

static int32_t size_map[PAGE_SIZE + 1];


static void size_map_init()
{
	int i = 0,j = 0;
	for (j = PAGE_SHIFT - 1; j >= 0 ; j--) {
		for (i = cache_size[j];i >= 0 ; i--) {
			size_map[i] = j;
		}
	}
	size_map[0] = 0;
}

static inline struct mem_cache_cpu* 
get_cpu_slab(struct mem_cache* c)
{
	return &c->cpu_slab[get_cpuid()];
}

static void percpu_cache_init(struct mem_cache* c)
{
	int i = 0;
	struct mem_cache_cpu * cpucache = NULL;
	for (;i < CPUNUMS ; i++) {
		cpucache = &c->cpu_slab[i];
		cpucache->freelist = NULL;
		cpucache->page  = NULL;
	}
}

static void node_cache_init(struct mem_cache* c)
{
	
	struct mem_cache_node *node = &c->node;
	INIT_LIST_HEAD(&node->full); 
	INIT_LIST_HEAD(&node->partial); 
	node->nslub = 0;
	node->npartial = 0;
	node->nfull = 0;
	SPIN_LOCK_INIT(&node->list_lock);
}

static void kmem_cache_init()
{
	int i = 0;
	struct mem_cache* cstart = kmalloc_caches;
	struct mem_cache* cend = kmalloc_caches + PAGE_SHIFT;
	for (i = 0,cstart; cstart < cend; cstart++,i++) {

		cstart->offset = ROUNDUP(cache_size[i],sizeof(void*))/sizeof(void*);
		cstart->size = ROUNDUP(cache_size[i],sizeof(void*)) + sizeof(void*);
		cstart->objsize = cache_size[i];
		cstart->order = SLABORDER;
		cstart->ctor = NULL;
		cstart->objects = (PAGE_SIZE << SLABORDER) / cstart->size;

		percpu_cache_init(cstart);
		node_cache_init(cstart);

		INIT_LIST_HEAD(&cstart->list);
#ifdef DEBUG
		printk("i:%d ",i);
		printk("offset:%d size:%d objsize:%d ",cstart->offset,cstart->size,cstart->objsize);
		printk("objects:%d\n",cstart->objects);
#endif
	}

	
}

void slab_init()
{
	size_map_init();
	
	kmem_cache_init();
}

static void new_slab_init(struct mem_cache *c,struct page* pf)
{
	assert(pf);
	pf->flags |= _pg_slab;
	pf->freelist = (void*)page2virt(pf);
	pf->inuse = 0;
	pf->slab = c;
	SPIN_LOCK_INIT(&pf->slublock);
	int i = 0;
	uint32_t off = c->offset;
	
	void* start = pf->freelist;
	void* end = start + c->objects*c->size;
	void** object;
	for(start ; start < end; start += c->size) {
		object = start;
		object[off] = start + c->size;
	}
	object[off] = NULL;
}

struct page* new_slab(struct mem_cache *c)
{
	struct page* page = NULL;
	struct mem_cache_node* n = &c->node;

	page = pages_alloc(_GFP_ZERO,c->order);

	if (!page)
		return NULL;
	

	new_slab_init(c,page);

	spin_lock(&n->list_lock);

	c->node.nslub++;

	spin_unlock(&n->list_lock);

	return page;
}


static void 
add_partial(struct mem_cache_node* n, struct page* page)
{
	spin_lock(&n->list_lock);

	n->npartial++;

	list_add(&page->list,&n->partial);
	
	spin_unlock(&n->list_lock);
}


static void 
remove_partial(struct mem_cache *c ,struct page* page)
{
	struct mem_cache_node* n = &c->node;

	spin_lock(&n->list_lock);

	n->npartial--;

	list_del(&page->list);

	spin_unlock(&n->list_lock);
}


static void 
slab_lock(struct page* page)
{
	spin_lock(&page->slublock);
}

static void 
slab_unlock(struct page* page)
{
	spin_unlock(&page->slublock);
}

static inline int 
slab_trylock(struct page* page)
{
	return spin_trylock(&page->slublock);
}


static inline int 
PageSlubFrozen(struct page* page)
{
	return page->flags & _pg_active;
}

static inline void 
ClearPageSlubFrozen(struct page *page)
{
	page->flags &=  ~_pg_active;
}

static inline void 
SetPageSlubFrozen(struct page *page)
{
	page->flags |=  _pg_active;
}

static inline int 
lock_and_freeze_slab(struct mem_cache_node *n, struct page *page)
{
	if (slab_trylock(page)) {
		list_del(&page->list);
		n->npartial--;
		SetPageSlubFrozen(page);
		slab_unlock(page);
		return 1;
	}
	return 0;
}


struct page* get_partial_slab(struct mem_cache *c)
{
	struct page* page = NULL;

	struct mem_cache_node *n = &c->node;

	spin_lock(&n->list_lock);

	if (!n->npartial)
		goto out;
	

	list_for_each_entry(page, &n->partial, list) {
		if (lock_and_freeze_slab(n, page))
			goto out;
	}

	page = NULL;

out:
	spin_unlock(&n->list_lock);

	return page;
}


static void 
discard_slab(struct mem_cache* c, struct page* page)
{
	struct mem_cache_node *n = &c->node;

	spin_lock(&n->list_lock);

	c->node.nslub--;

	spin_unlock(&n->list_lock);

	pages_free (page,c->order);

}

static void 
unfreeze_slab(struct mem_cache *c, struct page *page)
{  
	struct mem_cache_node *node = &c->node;  
	page->flags &=  ~_pg_active;
	if (page->inuse) {  
		if (page->freelist) {  
           		add_partial(node, page);  
        	} else { 
                	//add_full(node, page);  
       		} 
		slab_unlock(page);
	} else {  
		assert(0);
		slab_unlock(page);
        	discard_slab(c, page);
        } 
	
} 

static void 
deactivate_slab(struct mem_cache *c, struct mem_cache_cpu *cpucache)  
{  
	struct page *page = cpucache->page;  

	while (cpucache->freelist) {  
		void **object;  
		object = cpucache->freelist;  
		cpucache->freelist = cpucache->freelist[c->offset];  
		object[c->offset] = page->freelist;  
		page->freelist = object;  
		page->inuse--;  
		assert(0);
	}  
	cpucache->page = NULL;  
	unfreeze_slab(c, page);  
} 

static inline 
struct page* get_slab(void *obj)
{
	struct page *pf = NULL;
	viraddr_t virs = ROUNDDOWN((viraddr_t)obj,PAGE_SIZE *(1<<SLABORDER));
	return virt2page(virs); 
}

void* 
_slab_alloc(struct mem_cache *c,struct mem_cache_cpu* cpucache)
{
	struct page* new = NULL;
	void ** object = NULL;
	int i = 0;
	

	if (cpucache->page == NULL)
		goto new_slab;

load_slab:
	slab_lock(cpucache->page);

	object = cpucache->page->freelist;

	if (!object)
		goto deactivate_slab;

	cpucache->freelist = object[c->offset];
	cpucache->page->freelist = NULL;
	cpucache->page->inuse = c->objects;

	slab_unlock(cpucache->page);

	return object;

deactivate_slab:
	deactivate_slab(c, cpucache);  

new_slab:
	new = get_partial_slab(c);
	if (new) {
		cpucache->page = new;
		goto load_slab;
	}

	new = new_slab(c);
	if (new) { 
		cpucache->page = new;
		new->flags |=  _pg_active;
		goto load_slab;  
	}  
	return NULL;
}


void* 
slab_alloc(struct mem_cache* c)
{
	void **object = NULL;
	struct mem_cache_cpu* cpucache = &c->cpu_slab[get_cpuid()];
		
	if(!cpucache->freelist) {
		object = _slab_alloc(c,cpucache);
	} else {
		object = cpucache->freelist;
		cpucache->freelist = object[c->offset];
	}
	
	return object;		
}

static void 
_slab_free(struct mem_cache* c ,struct page* page, void **obj)
{
	void *prior;

	slab_lock(page);

	prior = obj[c->offset] = page->freelist;
	page->freelist = obj;
	page->inuse--;

	if (PageSlubFrozen(page)) {
		goto out_unlock;
	}
	
	if (!page->inuse)
		goto slab_empty;
	
	if (!prior) {
		assert(page->inuse == c->objects - 1);
        	add_partial(&c->node, page);
    	}  
out_unlock:  
	slab_unlock(page);  
	return; 


slab_empty:
	if (prior) {
       		remove_partial(c ,page); 
   	}  
	
	slab_unlock(page);
	discard_slab(c, page);
}


static void 
slab_free(struct mem_cache* c,void** obj, struct page* page)
{
	struct mem_cache_cpu *cachecpu = get_cpu_slab(c);

	if (!(page->flags & _pg_slab))
		return ;
	
	if (cachecpu->page == page) {
		obj[c->offset] = cachecpu->freelist;
		cachecpu->freelist = obj;
	} else {
		_slab_free(c,page,obj);
	}
	
}

static void 
get_slab_info_s(struct mem_cache* c)
{
	printk("size:%d objsize:%d objects:%d nfull:%d npartial:%d nslub:%d cpu:%d\n",
	c->size,c->objsize,c->objects,c->node.nfull,
	c->node.npartial,c->node.nslub,get_cpuid());
	
}


void 
get_slab_info()
{
	struct mem_cache* s = kmalloc_caches;
	struct mem_cache* e = kmalloc_caches + PAGE_SHIFT;
	for (; s < e; s++) {
		if(s->objsize == 2048)
			get_slab_info_s(s);
	}
}
void* 
kmalloc(size_t size)
{
	void* o = NULL;
	
	struct mem_cache *c = NULL;
	if(size <= 0)
		return NULL;
	if(size > PAGE_SIZE)
		return NULL;
	c = &kmalloc_caches[size_map[size]];
	return slab_alloc(c);
}

void 
kfree(void* obj)
{	
	if (obj) {
		struct page* page = get_slab(obj);
		if (!page) return;
		slab_free(page->slab,obj,page);
	}	
}



