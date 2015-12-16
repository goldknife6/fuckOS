#include <mm/mmzone.h>

#include <fuckOS/kernel.h>
#include <fuckOS/list.h>

#include <errno.h>


zone_t zone_normal;
zone_t zone_high;

static inline bool page_is_buddy(struct page *page, int order)  
{  
	
       if ((page_order(page) == order) &&  
            (page_count(page) == 0) &&
	    (!page_reserved(page)))
               return true;  
       return false;  
}  

inline void set_page_order(struct page *page, int order)  
{  
	page->private = order;
}



/*
*This function called by _alloc_buddy()
*
*@zone	
*@order	
*/

static page_t * __alloc_buddy (zone_t * zone,uint8_t low,uint8_t high,page_t * page)
{
	free_area_t *area = &zone->free_area[high];
	uint32_t size = 1UL << high;  

	while(low < high){
		high--;
		area--;
		size >>= 1UL;
		list_add(&page[size].list, &area->free_list);  
                area->nfree++;  
                set_page_order(&page[size], high); 
	}
	//spin_unlock(&zone->lock);
	return page;
}

/*
*This function called by alloc_buddy()
*
*@zone	
*@order	
*/

page_t * _alloc_buddy (zone_t * zone,uint8_t order)
{
	if (zone->zone_mem_map == NULL)
		return NULL;
	
	int curorder = 0;
	
	for (curorder = order; curorder < MAXORDER; curorder++) {
		if (list_empty(&zone->free_area[curorder].free_list))
			continue;

		struct list_head *pl = zone->free_area[curorder].free_list.next;
		page_t * page = list_entry(pl, page_t, list);
		list_del(pl);
		zone->free_area[curorder].nfree--;
		zone->free_pages -= (1UL << order);
		set_page_order(page,-1);
		return __alloc_buddy (zone,order,curorder,page);
	}
	return NULL;
}

page_t * alloc_buddy (zone_t * zone,size_t order)
{
	page_t * page = NULL;
	spin_lock(&zone->lock);
	page = _alloc_buddy (zone,order);
	spin_unlock(&zone->lock);
	return page;
}

static void __free_buddy (zone_t * zone,page_t *page,uint8_t order)
{
	uint32_t index = page - zone->zone_mem_map;
#ifdef DEBUG
	assert(zone);
	assert(page);
#endif
	zone->free_pages += 1UL << order; 

	while(order < (MAXORDER - 1)) {
		
		uint32_t buddy_index = index ^ (1UL << order);
		page_t *buddy = zone->zone_mem_map + buddy_index;

#ifdef DEBUG
	assert(buddy);
	assert(!(page2pfn(buddy) % (1<<order)));
#endif		
		if(buddy >= zone->zone_mem_map + zone->size)
			break;

		if(!page_is_buddy(buddy, order))
			break;
		free_area_t *area = &zone->free_area[order];
		area->nfree--;
		list_del(&buddy->list);
		set_page_order(buddy,-1);
		
		
		index &= buddy_index;
		
		order++;
		
	}
	page = zone->zone_mem_map + index;
	set_page_order(page,order);
	list_add(&page->list, &zone->free_area[order].free_list);
	zone->free_area[order].nfree++;

}

static void _free_buddy (zone_t * zone,page_t *page)
{


}

void free_buddy (page_t *page,uint8_t order)
{	
	zone_t * zone = &zone_normal;
	static int i = 0;
	spin_lock(&zone->lock);
	__free_buddy (&zone_normal,page,order);
	spin_unlock(&zone->lock);
}


static void free_area_init (zone_t * zone)
{
	if (zone->zone_mem_map == NULL)
		return;
	uint32_t i = 0,j = 0,np = 0;

	page_t* mmap = zone->zone_mem_map;

	for (i = 0 ;i<MAXORDER; i++)
		INIT_LIST_HEAD(&zone->free_area[i].free_list);

	for (i = 0 ;i<zone->size;i++) {
		if(page_order(&mmap[i]) == 0) {
			__free_buddy (zone,&mmap[i],0);
		}
	}
	list_for_each_entry(mmap, &zone->free_area[0].free_list, list) {
			j++;	
	}
	zone->free_area[0].nfree = j;
	
}

static void check_free_area (zone_t * zone)
{
	uint32_t i = 0,np = 0;
	uint32_t sumpages = 0;

	if (zone->zone_mem_map == NULL)
		return;
	
	for(i = 0;i < zone->size;i++) {
		if(page_count(&zone->zone_mem_map[i]) == 0) {
			np++;
		}
	}

	for(i = 0;i < MAXORDER; i++) {
		page_t* mmap;
		uint32_t j = 0;
		list_for_each_entry(mmap, &zone->free_area[i].free_list, list) {
			j++;
		}
		
		sumpages += (1 << i) * j;
	}
	zone->free_pages = np;
	printk("sumpages:%x np:%x\n",sumpages,np);
	assert(sumpages == np);
}

static void check_alloc_buddy (zone_t * zone)
{
	if (zone->zone_mem_map == NULL)
		return;

	uint32_t i = 0,np = 0;
	uint32_t sumpages = zone->free_pages;
	uint32_t sumpages2 = zone->free_pages;
		
	page_t *page = NULL;
	page_t *page2 = NULL;
	page_t *page3 = NULL;
	page_t *pp[11];
	struct list_head pagehead = LIST_HEAD_INIT(pagehead);
	for(i = 0; i < MAXORDER; i++) {
		sumpages -= 1 << i;
		pp[i] = _alloc_buddy (zone,i);
		assert(pp[i]);
		uint32_t j = 0;
		for(; j < 1 << i; j++) {
			assert(page_order(pp[i]) == -1);	
		}
		
	}

	printk("zone->free_pages:%d\n",zone->free_pages);

	
	assert(sumpages == zone->free_pages);
			
	for(i = 0; i < MAXORDER; i++) {
		__free_buddy (zone,pp[i],i);
	}
	
	assert(zone->free_pages == sumpages2);
	printk("zone->free_pages:%d\n",zone->free_pages);
}


void _zone_init ()
{
	pfn_t pfn = 0;
	pfn_t i = 0;
	
	zone_normal.zone_mem_map = mem_map;
	zone_normal.size = max_normal_pfn;
	
	free_area_init (&zone_normal);

	zone_high.size = highend_pfn - highstart_pfn;
	zone_high.zone_mem_map = highend_pfn ? mem_map + highstart_pfn : NULL;

	free_area_init (&zone_high);


#ifdef DEBUG
	check_free_area (&zone_normal);
	check_free_area (&zone_high);

	printk("zone_normal.free_pages:%08d\n",zone_normal.free_pages);
	printk("zone_normal.size:%08d\n",zone_normal.size);
	printk("zone_high.free_pages:%08d\n",zone_high.free_pages);
	printk("zone_high.size:%08d\n",zone_high.size);

#endif
	
	check_alloc_buddy (&zone_normal);
	check_alloc_buddy (&zone_high);
	
}	

void get_zone_info(struct zone_struct *zone)
{
	uint32_t np = zone->free_pages;
	int i;
	printk("CPU:%d free pages:%d %dMB\n",get_cpuid(),np, np * PAGE_SIZE /1024/1024);
	for (i = 0; i < MAXORDER; i++) {
		np = zone->free_area[i].nfree;
		//printk("order:%d np:%d %dMB \n",i,np,(PAGE_SIZE << i)* np/1024/1024);	
	}
}
