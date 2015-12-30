#ifndef _MINIOS_DCHCHE_H
#define _MINIOS_DCHCHE_H

#include <fuckOS/list.h>
#include <fuckOS/fs.h>
#include <fuckOS/namei.h>

#include <asm/atomic.h>
struct dentry;

struct dentry_operations {
    uint32_t (*d_hash)(const struct dentry *, const char*,int);
};


struct dentry {
	struct hlist_node d_hash;    /* lookup hash list */
	struct list_head d_subdirs;    /* lookup hash list */
	struct dentry *d_parent;    /* parent directory */
	const struct dentry_operations *d_op;
	struct inode *d_inode;
	struct super_block *d_sb;
	struct qstr d_name;
	atomic_t d_count;
};

static inline uint32_t _hash(const char* str,int len)
{
	uint32_t i,hash;
	for (i = 0; i < len; i++)
		hash = hash + (hash << 5) + str[i];
	return hash;
}
static inline struct dentry *dget(struct dentry *dentry)
{
	if (dentry) {
		atomic_inc(&dentry->d_count);
	}
	return dentry;
}

static inline void print_dentry(struct dentry *dentry)
{
	if (dentry) {
		printk("name:");
		print_qsrt(&dentry->d_name);
		printk("count:%d ",atomic_read(&dentry->d_count));
		printk("sb:%x ",dentry->d_sb);
		printk("inode:%x",dentry->d_inode);
		printk("\n");
	}
}




extern struct dentry *d_alloc_root(struct inode * root_inode);
extern struct dentry *root_dentry;

#endif/*!_MINIOS_DCHCHE_H*/
