#ifndef _MINIOS_DCHCHE_H
#define _MINIOS_DCHCHE_H

#include <fuckOS/list.h>
#include <fuckOS/fs.h>

#define DNAME_LEN_MIN 36


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
	char* d_name;
	int d_len;
};

static inline uint32_t _hash(const char* str,int len)
{
	uint32_t i,hash;
	for (i = 0; i < len; i++)
		hash = hash + (hash << 5) + str[i];
	return hash;
}


extern struct dentry *lookup_dentry(struct dentry *,const char *,int);

extern struct dentry *alloc_dentry(struct dentry *,char* ,
		struct dentry_operations *,struct inode *,
		struct super_block *,int);


#endif/*!_MINIOS_DCHCHE_H*/
