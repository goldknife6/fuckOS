#include <fuckOS/rootfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <mm/slab.h>


static struct super_block *get_root_super(struct file_system_type *,int);
static int rootfs_init(void*);

struct file_system_type root_fs = 
{
	.name = "rootfs",
	.get_super = get_root_super,
	.fs_init = rootfs_init,
	.next = LIST_HEAD_INIT(root_fs.next),
	.fs_supers = LIST_HEAD_INIT(root_fs.fs_supers),
};

static int rootfs_init(void *v)
{
	printk("registering %s file system!\n",root_fs.name);

	struct super_block *sb;
	struct vfsmount *mnt;

	mnt = alloc_vfsmnt(root_fs.name);
	assert(mnt);

	sb = get_root_super(&root_fs,0);

	list_add(&sb->s_list,&root_fs.fs_supers);

	sb->s_op->read_super(sb);
	return 0;
}

static struct super_block * 
get_root_super(struct file_system_type *fs,int dev)
{
	struct super_block *sb;
	sb = alloc_super_block(dev,0,&root_super_op,NULL,0,0);
	assert(sb);
	return sb;
}
