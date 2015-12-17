#include <mm/mmzone.h>

#include <fuckOS/kernel.h>
#include <fuckOS/list.h>

#include <errno.h>


struct zone_struct zone_normal;
struct zone_struct zone_high;

#define page_order(page) 		((page)->order)
#define page_count(page) 		(atomic_read(&(page)->nref))
#define page_order_inc(page) 		(atomic_inc(&(page)->nref))
#define page_order_dec(page) 		(atomic_dec(&(page)->nref))
#define page_reserved(page)		(((page)->flags) & _pg_reserved)

static inline bool 
page_is_buddy(struct page *page, uint8_t order)  
{  
#ifdef CONFIG_DEBUG
	assert(page);
	assert(order >= 0);
	assert(order < MAXORDER);
#endif	
       if ((page_order(page) == order) &&  
            (page_count(page) == 0) &&
	    (!page_reserved(page)))
               return true;  
       return false;  
}

static struct page *
__alloc_buddy(struct zone_struct* zone,
			uint8_t low,uint8_t high,struct page *page)
{
	struct free_area_struct *area;
	area = &zone->free_area[high];

	uint32_t size = 1UL << high;  

	while(low < high){
		high--;
		area--;
		size >>= 1UL;
		list_add(&page[size].list, &area->free_list);  
                area->nfree++;  
                page->order = high; 
	}
	return page;
}

static struct page *
_alloc_buddy(struct zone_struct *zone,uint8_t order)
{
	struct list_head *pl;
	struct page *page;
	int curorder;

#ifdef CONFIG_DEBUG
	assert(zone);
	assert(order >= 0);
	assert(order < MAXORDER);
#endif

	if (zone->size == 0)
		return NULL;
	
	for (curorder = order; curorder < MAXORDER; curorder++) {
		if (list_empty(&zone->free_area[curorder].free_list))
			continue;

		pl = zone->free_area[curorder].free_list.next;
		page = list_entry(pl, struct page, list);
		list_del(pl);
		zone->free_area[curorder].nfree--;
		zone->free_pages -= (1UL << order);
		page->order = -1;
		return __alloc_buddy(zone,order,curorder,page);
	}

	return NULL;
}

struct page* 
alloc_buddy(struct zone_struct *zone,uint8_t order)
{
	struct page* page = NULL;
	spin_lock(&zone->lock);
	page = _alloc_buddy(zone,order);
	spin_unlock(&zone->lock);
	return page;
}

void 
free_buddy(struct zone_struct *zone,
			struct page* page,uint8_t order)
{
	struct free_area_struct *area;
	uint32_t index = page - zone->zone_mempage;

#ifdef CONFIG_DEBUG
	assert(index >= 0);
	assert(zone);
	assert(page);
	assert(order >= 0);
	assert(order < MAXORDER);
#endif
	zone->free_pages += 1 << order; 

	while (order < MAXORDER - 1) {
		
		int32_t buddy_index = index ^ (1 << order);
		struct page* buddy = zone->zone_mempage + buddy_index;

		if (buddy >= zone->zone_mempage + zone->size)
			break;
		if (buddy < zone->zone_mempage)
			break;

#ifdef CONFIG_DEBUG
	assert(buddy);
	assert(!(page2pfn(buddy) % (1<<order)));
#endif		

		if (!page_is_buddy(buddy, order))
			break;
		area = &zone->free_area[order];
		area->nfree--;
		list_del(&buddy->list);

		buddy->order = -1;
		
		index &= buddy_index;
		
		order++;
		
	}
	page = zone->zone_mempage + index;
	page->order = order;
	
	list_add(&page->list, &zone->free_area[order].free_list);
	zone->free_area[order].nfree++;
}


static void 
free_area_init(struct zone_struct * zone)
{
	int32_t i = 0,j = 0,np = 0;

	struct page* mmap = zone->zone_mempage;

	for (i = 0 ;i< MAXORDER; i++)
		INIT_LIST_HEAD(&zone->free_area[i].free_list);

	for (i = 0 ;i < zone->size;i++) {
		if(page_order(&mmap[i]) == -1 && mmap[i].flags & _pg_free) {
			free_buddy(zone,&mmap[i],0);
		}
	}

}

static void buddy_check()
{
	int c = 0,i;
	struct page* page;
	physaddr_t phys;

	while(1) {
		page = alloc_buddy(&zone_normal,0);
		if(!page)
			break;
		phys = page2phys(page);
		if(phys < 0x100000 || PFN(phys) >= normal_maxpfn)
			panic("normal\n");
	}

	while(1) {
		page = alloc_buddy(&zone_high,0);
		if(!page)
			break;
		phys = page2phys(page);
		if(phys < 0x30000000 || PFN(phys) >= high_maxpfn)
			panic("normal\n");
	}
	panic("down\n");
}
/*
*
*	初始化伙伴系统
*
*/
void zone_init()
{

	zone_normal.size = normal_maxpfn;
	zone_normal.zone_mempage = mempage;
	zone_normal.free_pages = 0;
	SPIN_LOCK_INIT(&zone_normal.lock);

	zone_high.size = max_pfn - normal_maxpfn;
	zone_high.zone_mempage = zone_high.size ? mempage + normal_maxpfn : NULL;
	zone_high.free_pages = 0;
	SPIN_LOCK_INIT(&zone_high.lock);

	free_area_init (&zone_normal);

	free_area_init(&zone_high);
	
	buddy_check();

}
