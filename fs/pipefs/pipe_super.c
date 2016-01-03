#include <fuckOS/pipe.h>
#include <fuckOS/assert.h>
#include <fuckOS/libfs.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int pipefs_read_super(struct super_block *,void*,int);
extern struct vfsmount *root_vfsmnt;
struct super_operations pipefs_super_op = 
{
	.read_super = pipefs_read_super,
};



static int pipefs_read_super(struct super_block *sb,
				void* data,int slient)
{
	struct dentry *root;
	struct inode *inode = NULL;
	struct vfsmount* mnt = (struct vfsmount*)data;

	inode = simple_get_inode(sb, S_IFDIR, sb->s_dev);

	assert(inode);
	root = d_alloc(root_vfsmnt->mnt_mountpoint,&(const struct qstr){"pipefs",6});
	assert(root);


	mnt->mnt_sb = sb;
	mnt->mnt_mountpoint = root;
	mnt->mnt_root = root_vfsmnt->mnt_mountpoint;
	mnt->mnt_parent = root_vfsmnt;
	return 0;
}
