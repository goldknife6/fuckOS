#ifndef _BUDDY_H_
#define _BUDDY_H_ 

typedef struct free_area_struct {
         struct list_head        free_list;
         unsigned long		*map;
} free_area_t; 


#endif/*!_BUDDY_H_ */
