#include <fuckOS/dcache.h>
#include <fuckOS/list.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>

#define MAX_DENTRY_INODE	1024

static struct hlist_head dentry_hashtable[MAX_DENTRY_HASH];
STATIC_INIT_SPIN_LOCK(dcache_lock)
struct dentry *root_dentry;


struct dentry *d_alloc_root(struct inode * root_inode)
{
	struct dentry *res = NULL;
	if (root_inode) {
		res = d_alloc(NULL, &(const struct qstr) { "/", 1});
		if (res) {
			res->d_sb = root_inode->i_sb;
			res->d_parent = res;
			res->d_inode = root_inode;
			root_dentry = res;
		}
	}
	return res;
}

struct dentry* dentry_lookup(struct dentry *parent,struct qstr *qstr)
{
	uint32_t h;
	struct hlist_head *hlist;
	struct dentry *d;
	h = hash(parent,qstr->name,qstr->len);
	hlist = &dentry_hashtable[h%MAX_DENTRY_HASH];
	hlist_for_each_entry(d, hlist, d_hash){
		if (!strncmp(d->d_name.name,qstr->name,qstr->len)) {
			return d;
		}
	}
	return NULL;
}

void dentry_insert(struct dentry *parent,struct dentry *dentry)
{
	uint32_t h;
	struct hlist_head *hlist;
	h = hash(parent,dentry->d_name.name,dentry->d_name.len);
	hlist = &dentry_hashtable[h%MAX_DENTRY_HASH];
	hlist_add_head(&dentry->d_hash, hlist);
	printk("insert parent:%x name:%s len:%d\n",parent,dentry->d_name.name,dentry->d_name.len);
}

struct dentry * 
d_alloc(struct dentry * parent, const struct qstr *name)
{
	struct dentry *dentry;
	char * str;
	dentry = kmalloc(sizeof(struct dentry));
	if (!dentry)
		return NULL;
	dentry->d_name = *name;
	dentry->d_sb = NULL;
	dentry->d_parent = parent;
	dentry->d_inode = NULL;
	dentry->d_op = NULL;
	dentry->d_mounted = 0;
	INIT_LIST_HEAD(&dentry->d_subdirs);
	INIT_HLIST_NODE(&dentry->d_hash);
	atomic_set(&dentry->d_count, 1);
	if (parent) {
		dentry->d_parent = dget(dentry);
		dentry->d_sb = parent->d_sb;
	}
	return dentry;
}

int d_mountpoint(struct dentry *dentry)
{
	return dentry->d_mounted;
}
