#include <fuckOS/rootfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <mm/slab.h>

static int ttyfs_init(void*);
static struct super_block *ttyfs_get_super(struct file_system_type *,int ,void *);

struct file_system_type tty_fs = 
{
	.name = "ttyfs",
	.get_super = ttyfs_get_super,
	.fs_init = ttyfs_init,
	.next = LIST_HEAD_INIT(tty_fs.next),
	.fs_supers = LIST_HEAD_INIT(tty_fs.fs_supers),
};

static int ttyfs_init(void *data)
{
	printk("registering %s file system!\n",tty_fs.name);




	return 0;
}

static struct super_block *
ttyfs_get_super(struct file_system_type *fs_type,int dev,void *data)
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

