#include <fuckOS/minixfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <mm/slab.h>


static struct super_block *get_minix_super(struct file_system_type *,int);
static int minix_init(void*);

struct file_system_type minix_fs = 
{
	.name = "minix1",
	//.get_super = get_minix_super,
	.fs_init = minix_init,
	.next = LIST_HEAD_INIT(minix_fs.next),
	.fs_supers = LIST_HEAD_INIT(minix_fs.fs_supers),
};

static int minix_init(void *v)
{
	printk("registering %s file system!\n",minix_fs.name);
	return 0;
}

static struct super_block * 
get_minix_super(struct file_system_type *fs,int dev)
{
	struct minix_superblock* msb = NULL;
	struct buffer_head *buf = NULL;
	struct super_block* sb;
	
	return NULL;
}
