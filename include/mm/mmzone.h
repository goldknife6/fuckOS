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
	struct page* 	zone_mempage;
	uint32_t 	size;
	struct free_area_struct	free_area[MAXORDER];
};


extern struct zone_struct zone_normal;
extern struct zone_struct zone_high;


extern struct page* alloc_buddy(struct zone_struct *,uint8_t);
extern void free_buddy(struct zone_struct *,struct page*,uint8_t);
#endif/*!_MMZONE_H_*/
