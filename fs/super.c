#include <fuckOS/fs.h>
#include <fuckOS/minixfs.h>
#include <drivers/hd.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>
struct list_head super_list = LIST_HEAD_INIT(super_list);


static struct super_block *mount_root_super(int );
static int print_super(struct minix_superblock* s);

struct inode *
get_inode(struct super_block *sb)
{
	assert(sb);
	assert(sb->s_op);

	return sb->s_op->alloc_inode(sb);
}

struct super_block *get_super(int dev)
{
	struct super_block *s;

	if (!dev)
		return NULL;

	list_for_each_entry(s, &super_list, s_list) {
		if (s->s_dev == dev)
			return s;
	}

	return NULL;
}

void insert_super(struct super_block *sb)
{
	list_add(&sb->s_list, &super_list);
}

int read_inode(struct super_block *sb,struct inode *inode)
{
	assert(sb);
	assert(sb->s_op);
	assert(sb->s_op->read_inode);
	assert(inode);
	return sb->s_op->read_inode(inode);
}

int read_super(struct super_block *sb)
{
	assert(sb);
	assert(sb->s_op);
	assert(sb->s_op->read_super);
	assert(sb);
	return sb->s_op->read_super(sb);
}

static struct super_block *
mount_root_super(int dev)
{
	struct minix_superblock* msb = NULL;
	struct super_block* sb = NULL;
	struct buffer_head *buf = NULL;


	buf = buffer_read(dev,1);
	assert(buf);

	msb = (struct minix_superblock *)buf->b_data;

	print_super(msb);
	
	if (msb->s_magic != 0x138F)
		return NULL;


	//sb = alloc_super(dev,msb->s_magic,&minix_super_op,msb,
	//		1024*(msb->s_log_zone_size << 1),msb->s_max_size);

	assert(sb);

	return sb;
}

struct super_block*
alloc_super_block(int dev,uint16_t magic,
		const struct super_operations *op,void *super,
			int blocksize,int max_size)
{
	struct super_block* sb;
	sb = kmalloc(sizeof(struct super_block));

	if (!sb)
		return NULL;

	memset(sb,0,sizeof(struct super_block));
	INIT_LIST_HEAD(&sb->s_list);
	sb->s_dev = dev;
	sb->s_magic = magic;
	sb->s_op = op;
	sb->s_super = super;
	sb->s_blocksize = blocksize;
	sb->s_max_size = max_size;
	return sb;
}

static int print_super(struct minix_superblock* s)
{
	assert(s);

	printk("s_ninodes:%d s_nzones:%d\n",s->s_ninodes,s->s_nzones);
	printk("s_imap_blocks:%d s_zmap_blocks:%d\n",s->s_imap_blocks,s->s_zmap_blocks);
	printk("s_firstdatazone:%d s_log_zone_size:%d\n",s->s_firstdatazone,s->s_log_zone_size);
	printk("s_max_size:%d s_magic:%x\n",s->s_max_size,s->s_magic);
	return 0;
}
