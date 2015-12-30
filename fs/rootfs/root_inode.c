#include <fuckOS/fs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

struct dentry *rootfs_lookup(struct inode *,struct dentry *, struct nameidata*);

struct inode_operations rootfs_inode_op = 
{
	.lookup = rootfs_lookup,
};


struct dentry *rootfs_lookup(struct inode *dir, 
			struct dentry *dentry,struct nameidata* nd)
{
	struct dentry *d;
	
	return NULL;
}
