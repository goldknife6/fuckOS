#include <fuckOS/dcache.h>
#include <fuckOS/list.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>

#define MAX_DENTRY_INODE	1024


static struct hlist_head dentry_hashtable[MAX_DENTRY_HASH];


void insert_dentry(struct dentry *d)
{
	struct hlist_head *ht;
	uint32_t hash;

	hash = d->d_op->d_hash(d->d_parent,d->d_name,d->d_len);
	
	ht = &dentry_hashtable[hash % MAX_DENTRY_HASH];
	
}


struct dentry *alloc_dentry(struct dentry *p,char* name,
		struct dentry_operations *op,struct inode *inode,
		struct super_block *sb,int len)
{
	struct dentry *d;
	d = kmalloc(sizeof(struct dentry));
	if (!d)
		return NULL;

	memset(d,0,sizeof(struct dentry));
	INIT_HLIST_NODE(&d->d_hash);
	INIT_LIST_HEAD(&d->d_subdirs);
	d->d_parent = p;
	d->d_op = op;
	d->d_inode = inode;
	d->d_sb = sb;
	strncpy(d->d_name,name,len);
	d->d_len = len;
	return d;
}

struct dentry *lookup_dentry(struct dentry *parent,
				const char *name,int len)
{
	uint32_t hash;
	struct dentry *d;
	struct hlist_head* hlist;
	assert(parent);
	assert(name);
	if (len <= 0)
		return NULL;
	
	hash = parent->d_op->d_hash(parent,name,len);
	hlist = &dentry_hashtable[hash % MAX_DENTRY_HASH];

	hlist_for_each_entry(d, hlist, d_hash) {
		if (!strncmp(d->d_name,name,len))
			return d;
	}

	return NULL;
}


