#ifndef _MMZONE_H_
#define _MMZONE_H_ 

#include <fuckOS/list.h>
#include <mm/pages.h>

#define	MAXORDER	11

#define ZONE_NORMAL	1
#define ZONE_HIGHMEM	2

struct free_area_struct {
	struct list_head 	free_list;
	uint32_t 		nfree;
};


struct zone_struct {
	uint32_t 	free_pages;
	spinlock_t 	lock;
	page_t* 	zone_mem_map;
	uint32_t 	size;
	free_area_t	free_area[MAXORDER];
};


extern zone_t zone_normal;
extern zone_t zone_high;

#endif/*!_MMZONE_H_*/
