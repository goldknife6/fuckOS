#include <fuckOS/rootfs.h>
#include <fuckOS/ramfs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int ttyfs_read_super(struct super_block *,void*,int);

struct super_operations ttyfs_super_op = 
{
	.read_super = ttyfs_read_super,
};


static int ttyfs_read_super(struct super_block *sb,
				void* data,int slient)
{
	struct dentry *root;
	struct inode *inode = NULL;
	struct vfsmount* mnt = (struct vfsmount*)data;
	inode = ramfs_get_inode(sb, S_IFDIR, sb->s_dev);
	return 0;
}
