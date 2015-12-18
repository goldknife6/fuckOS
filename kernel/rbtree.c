#include <fuckOS/rbtree.h>
#include <fuckOS/assert.h>
#include <fuckOS/kernel.h>

static inline void 
rb_set_parent(struct rb_node *rb, struct rb_node *p)  
{  
         rb->rb_parent = p;  
}



static struct rb_node *
rb_mostleft(struct rb_node* rbnode)
{

	while(rbnode) {
		if(rbnode->rb_left)
			rbnode = rbnode->rb_left;
		else
			return rbnode;
	}
	return NULL;
}

static struct rb_node *
rb_mostright(struct rb_node* rbnode)
{
	while(rbnode) {
		if(rbnode->rb_right)
			rbnode = rbnode->rb_right;
		else
			return rbnode;
	}
	return NULL;
}

//它把parent设为node的父结点，并且让rb_link指向node。
inline void 
rb_link_node(struct rb_node* node, struct rb_node* parent, struct rb_node** rb_link)
{
	*rb_link = node;
	node->rb_parent = parent;
}

inline void 
RBTREE_ROOT_INIT(struct rb_root* rbroot)
{
	rbroot->rb_node = NULL;
}

inline void 
RBTREE_NODE_INIT(struct rb_node* rbnode)
{
	rbnode->rb_parent = NULL;
	rbnode->rb_right = NULL;
	rbnode->rb_left = NULL;
}


void 
rbtree_delete(struct rb_root* rbroot, struct rb_node* rbnode)
{

#ifdef DEBUG
	assert(rbnode);
#endif
	struct rb_node* left = rbnode->rb_left;
	struct rb_node* right = rbnode->rb_right;
	struct rb_node* parent = rb_parent(rbnode);
	struct rb_node** pp = NULL;


	if (!parent) 
		pp = &rbroot->rb_node;
	else if (parent->rb_left == rbnode)
		pp = &parent->rb_left;
	else if (parent->rb_right == rbnode)
		pp = &parent->rb_right;
	else
		panic("rbtree_delete\n");

	if (!left && !right)
		*pp = NULL;
	else if (!left) {
		*pp = right;
		right->rb_parent = parent;
	} else if (!right) {
		*pp = left;
		left->rb_parent = parent;
	} else {
		struct rb_node* tmp = rb_mostleft(right);
		rbtree_delete(rbroot,tmp);
		tmp->rb_left = left;
		tmp->rb_right = right;
		*pp = tmp;
	}
	rbnode->rb_left = rbnode->rb_right = NULL;
}

struct rb_node *
rb_first(struct rb_root *root)
{
	return rb_mostright(root->rb_node);
}

struct rb_node *
rb_last(struct rb_root *root)
{
	return rb_mostleft(root->rb_node);
}

