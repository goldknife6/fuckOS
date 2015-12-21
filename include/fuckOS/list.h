#ifndef _MINIOS_LISH_H
#define _MINIOS_LISH_H
#include <types.h>

struct list_head {
	struct list_head *next, *prev;
};

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)


#define LIST_HEAD_INIT(name) { &(name), &(name) } 


#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name);


#define list_for_each(pos, head) 				  \
	    for (pos = (head)->next; pos != (head); pos = pos->next) 


#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)


#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(pos, member))


#define list_for_each_entry(pos, head, member)                    \
     for (pos = list_entry((head)->next, typeof(*pos), member);   \
          &pos->member != (head);      				  \
          pos = list_entry(pos->member.next, typeof(*pos), member))
static inline void INIT_LIST_HEAD(struct list_head *list)  
{  
	list->next = list;  
	list->prev = list;  
}

static inline void __list_add(struct list_head *new,

			      struct list_head *prev,

			      struct list_head *next)

{

	next->prev = new;

	new->next = next;

	new->prev = prev;

	prev->next = new;

}

static inline void list_add(struct list_head *new, struct list_head *head)

{

	__list_add(new, head, head->next);

}


static inline void list_add_tail(struct list_head *new, struct list_head *head)  
{  
	__list_add(new, head->prev, head);  
}

static inline void __list_del(struct list_head * prev, struct list_head * next)  
{  
	next->prev = prev;  
	prev->next = next;  
}

static inline void list_del(struct list_head *entry)  
{  
	__list_del(entry->prev, entry->next);  
	entry->next = entry;  
	entry->prev = entry;  
} 

static inline int list_empty(const struct list_head *head)  
{  
	return head->next == head;  
} 

static inline int list_is_last(const struct list_head *list,
					const struct list_head *head)
{
	return list->next == head;
}

static inline int list_is_first(const struct list_head *list,
					const struct list_head *head)
{
	return list->prev == head;
}

#endif/*_MINIOS_LISH_H*/
