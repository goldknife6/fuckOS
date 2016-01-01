#include <fuckOS/libfs.h>
#include <fuckOS/ramfs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int root_read_super(struct super_block *,void*,int);

struct super_operations root_super_op = 
{
	.read_super = root_read_super,
};


static int root_read_super(struct super_block *sb,
				void* data,int slient)
{
	struct dentry *root;
	struct inode *inode = NULL;
	struct vfsmount* mnt = (struct vfsmount*)data;

	inode = simple_get_inode(sb, S_IFDIR, sb->s_dev);

	printk("alloc root inode 0x%x mode:%o ",inode,inode->i_mode);
	assert(inode);
	root = d_alloc_root(inode);
	assert(root);

	printk("alloc root dentry 0x%x ",root);

	mnt->mnt_sb = sb;
	mnt->mnt_mountpoint = root;
	mnt->mnt_root = root;
	mnt->mnt_parent = mnt;
	return 0;
}
