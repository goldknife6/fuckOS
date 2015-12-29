#include <fuckOS/minixfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <mm/slab.h>


static struct super_block *get_minix_super(struct file_system_type *,int);
static int minix_init(void*);

struct file_system_type minix_fs = 
{
	.name = "minix1",
	.get_super = get_minix_super,
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
	sb = find_super_block(fs,dev);
	if (sb)
		return sb;

	buf = buffer_read(dev,1);
	assert(buf);

	msb = kmalloc(sizeof(struct minix_superblock));
	assert(msb);

	*msb = *(struct minix_superblock *)buf->b_data;

	assert(msb->s_magic == 0x138F);

	sb = alloc_super_block(dev ,msb->s_magic ,&minix_super_op,msb,
		1024*(msb->s_log_zone_size << 1),msb->s_max_size);

	assert(sb);

	list_add(&sb->s_list,&fs->fs_supers);
	return sb;
}
