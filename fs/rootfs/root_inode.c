#include <fuckOS/fs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

struct dentry *rootfs_lookup(struct inode *,struct dentry *, struct nameidata*);
static int rootfs_file_read(struct file *,char* , int ,int );
static int rootfs_inode_create(struct inode *,struct dentry *, int, struct nameidata*);

struct inode_operations rootfs_inode_op = 
{
	.create = rootfs_inode_create,
	.lookup = rootfs_lookup,
};

struct file_operations rootfs_file_op = 
{
	.read = rootfs_file_read,
};

static int rootfs_inode_create(struct inode *dir,
		struct dentry *dentry, int mode, struct nameidata* nd)
{
	return 0;
}
struct dentry *rootfs_lookup(struct inode *dir, 
			struct dentry *dentry,struct nameidata* nd)
{
	struct dentry *d;
	
	return NULL;
}

static int rootfs_file_read(struct file *file,
			char* buf, int count,int offset)
{
	int i;
	printk("This is rootfs!!!!! Are you kidding me?\n");
	for (i = 0; i < count; i++) {
		buf[i] = 'c';
	}
	return i;
}
