#include <fuckOS/rootfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <mm/slab.h>

extern struct vfsmount *root_vfsmnt;
static int rootfs_init(void*);
static struct super_block *rootfs_get_super(struct file_system_type *,int ,void *);

struct file_system_type root_fs = 
{
	.name = "rootfs",
	.get_super = rootfs_get_super,
	.fs_init = rootfs_init,
	.next = LIST_HEAD_INIT(root_fs.next),
	.fs_supers = LIST_HEAD_INIT(root_fs.fs_supers),
};

static int rootfs_init(void *v)
{
	printk("registering %s file system!\n",root_fs.name);

	struct super_block *sb;
	struct vfsmount *mnt;
	int res;
	mnt = alloc_vfsmnt(root_fs.name);
	assert(mnt);
	root_vfsmnt = mnt;
	sb = root_fs.get_super(&root_fs,0,NULL);

	assert(sb);

	list_add(&sb->s_list,&root_fs.fs_supers);
	res = sb->s_op->read_super(sb,mnt,0);
	assert(res == 0);

	print_dentry(root_dentry);
	return 0;
}

static struct super_block *
rootfs_get_super(struct file_system_type *fs_type,int dev,void *data)
{
	struct super_block *s;
	s = find_super_block(fs_type,dev);
	if (s)
		return s;
	s = get_sb_nodev(fs_type,dev,data);
	s->s_op = &root_super_op;
	s->s_blocksize = PAGE_SIZE;
	s->s_magic = ROOTFS_MAGIC;
	return s;
}

