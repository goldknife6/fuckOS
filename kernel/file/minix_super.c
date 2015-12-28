#include <fuckOS/fs.h>
#include <fuckOS/minixfs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>


static void destroy_minix_inode(struct inode *);
static struct inode *alloc_minix_inode(struct super_block *);
static int read_minix_inode(struct inode *);
static int read_minix_super(struct super_block *);


struct super_operations minix_super_op = 
{
	.alloc_inode = alloc_minix_inode,
	.destroy_inode = destroy_minix_inode,
	.read_inode = read_minix_inode,
	.read_super = read_minix_super,
};


static int read_minix_super(struct super_block *sb)
{
	struct minix_superblock* msb = NULL;
	struct buffer_head *buf = NULL;

	buf = buffer_read(sb->s_dev,1);
	assert(buf);

	msb = (struct minix_superblock *)buf->b_data;

	if (msb->s_magic != 0x138F)
		panic("msb->s_magic != 0x138F");

	*(struct minix_superblock *)sb->s_super = *msb;
	
	return 0;
}

static struct super_block *get_minix_super(int dev)
{
	struct super_block *sb;
	struct minix_superblock* msb = NULL;
	sb = get_super(dev);
	if (sb)
		return sb;
	msb = kmalloc(sizeof(struct minix_superblock));
	sb = alloc_super(dev, 0x138F ,&minix_super_op,(void *)msb,
				1024*(msb->s_log_zone_size << 1),msb->s_max_size);
	return 0;
}

static int read_minix_inode(struct inode *inode)
{
	int block;
	assert(inode);
	struct super_block *sb = NULL;
	struct minix_superblock *msb = NULL;
	struct minix1_inode *minode;
	struct buffer_head * buf;
	
	sb = inode->i_sb;
	assert(sb);
	msb = (struct minix_superblock *)sb->s_super;
	assert(msb);
	minode = (struct minix1_inode *)inode->i_inode;
	assert(minode);

	block = 2 + msb->s_imap_blocks + msb->s_zmap_blocks + inode->i_num / MINIX1_INODES_PER_BLOCK;
	
	if (!(buf = buffer_read(inode->i_dev,block)))
		panic("unable to read i-node block");

	*(struct minix1_inode *)inode->i_inode = 
		((struct minix1_inode *)buf->b_data)[inode->i_num % MINIX1_INODES_PER_BLOCK];
	//printk("num:%d dev:%x block:%d\n",inode->i_num,inode->i_dev,block);
	return 0;
}

static void
destroy_minix_inode(struct inode *inode)
{
	
}

static struct inode *
alloc_minix_inode(struct super_block *sb)
{
	assert(sb);
	assert(sb->s_super);

	struct minix_superblock *msb = NULL;
	struct inode *inode = NULL;
	msb = (struct minix_superblock *)sb->s_super;





	return inode;
}

