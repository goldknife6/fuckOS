#include <lib.h>
#include <stdio.h>
			//4M
#define CHUNK_SIZE	0x400000
#define CHUNK_GAP	0x1000
#define CHUNK_FREE	1
#define CHUNK_INUSE	2

#define MC_DATA(mc)	({\
	(uint8_t*)mc + sizeof(struct malloc_chunk);\
})

#define MC_STRUCT(data)	({\
	(struct malloc_chunk *)((uint8_t*)data - sizeof(struct malloc_chunk));\
})

static struct malloc_chunk *find_malloc(uint32_t);
static struct malloc_chunk *extend_malloc(uint32_t);
static void split_malloc(struct malloc_chunk *,uint32_t);
static void merger_malloc(struct malloc_chunk *);

extern void *_startbrk;
extern void *_curbrk;

struct list_head free_list = LIST_HEAD_INIT(free_list);


void *malloc(uint32_t size)
{

	struct malloc_chunk *mc;

find_mc:
	mc = find_malloc(size);

	if (!mc) {
		mc = extend_malloc(CHUNK_SIZE);
		if (!mc)
			return NULL;
		goto find_mc;
	}


	split_malloc(mc,size);

	mc->flag = CHUNK_INUSE;
	
	return MC_DATA(mc);
}

void free(void *obj)
{
	struct malloc_chunk *mc;
	if (!obj)
		return;

	mc = MC_STRUCT(obj);
	mc->flag = CHUNK_FREE;
	merger_malloc(mc);
}

static struct malloc_chunk *
find_malloc(uint32_t size)
{
	struct malloc_chunk *mc = NULL;
	list_for_each_entry_reverse(mc, &free_list, list) {
		if(mc->flag == CHUNK_FREE && mc->size >= size)
			return mc;
	}	
	return NULL;
}

static void
split_malloc(struct malloc_chunk *mc,uint32_t size)
{
	struct malloc_chunk *newmc;
	if (mc->size - size < CHUNK_GAP)
		return;
	
	newmc = (struct malloc_chunk *)((char*)mc + size + sizeof(struct malloc_chunk));
	newmc->size = mc->size - size - sizeof(struct malloc_chunk);
	newmc->flag = CHUNK_FREE;
	INIT_LIST_HEAD(&newmc->list);
	list_add(&newmc->list,&mc->list);

	mc->size = size;
}

static void
merger_malloc(struct malloc_chunk *mc)
{
	struct malloc_chunk *prev = NULL,*next = NULL;

	if (!list_is_last(&mc->list,&free_list))
		next = list_entry(mc->list.next,struct malloc_chunk,list);

	if (!list_is_first(&mc->list,&free_list))
		prev = list_entry(mc->list.prev,struct malloc_chunk,list);

	if (prev && prev->flag == CHUNK_FREE) {
		prev->size += mc->size + sizeof(struct malloc_chunk);
		list_del(&mc->list);
		mc = prev;
	}

	if (next && next->flag == CHUNK_FREE) {
		mc->size += next->size + sizeof(struct malloc_chunk);
		list_del(&next->list);
	}
}

static struct malloc_chunk *
extend_malloc(uint32_t size)
{
	viraddr_t va;
	struct malloc_chunk*mc = NULL;

	va = sbrk(CHUNK_SIZE);
	if (va) {
		mc = (struct malloc_chunk*)va;
		mc->size = CHUNK_SIZE - sizeof(struct malloc_chunk);
		mc->flag = CHUNK_FREE;
		INIT_LIST_HEAD(&mc->list);
		list_add_tail(&mc->list,&free_list);
		merger_malloc(mc);
	}
	return mc;
}
