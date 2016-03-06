#ifndef _MINIOS_RBTREE_H_
#define _MINIOS_RBTREE_H_ 
#include <types.h>

#define RB_RED 0
#define RB_BLACK 1

#if defined(container_of)
  #undef container_of
  #define container_of(ptr, type, member) ({            \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#if defined(offsetof)
  #undef offsetof
  #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define RB_ROOT {NULL}

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define rb_parent(r) ((struct rb_node*)((r)->rb_parent))


struct rb_node
{
	//uint32_t rb_parent_color;
	struct rb_node *rb_parent;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
};//__attribute__((aligned(sizeof(long))));


struct rb_root
{
  struct rb_node *rb_node;
};

extern void RBTREE_ROOT_INIT(struct rb_root* rbroot);
extern void RBTREE_NODE_INIT(struct rb_node* rbnode);
extern void rbtree_delete(struct rb_root* rbroot, struct rb_node* rbnode);
extern void rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link);
extern struct rb_node *rb_first(struct rb_root *root);
#endif/*!_MINIOS_RBTREE_H_*/
