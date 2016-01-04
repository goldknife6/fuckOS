#include <fuckOS/minixfs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static int minix_file_write(struct file *,char* , int ,int );
static int minix_file_read(struct file *,char* , int ,int );
static int minix_file_open(struct inode *,struct file *);

static struct dentry* minix_lookup(struct inode *dir,struct dentry *dentry,struct nameidata* nd);
static int _bmap(struct inode *,int);

static void put_fs_byte(char c,char* p)
{
	*p = c;
}


struct inode_operations minix_inode_op = 
{
	
};

struct file_operations minix_file_op = 
{
	.read = minix_file_read,
	.open = minix_file_open,
	.write = minix_file_write,
};



struct dentry *minix_lookup(struct inode *dir, 
			struct dentry *dentry,struct nameidata* nd)
{
	return NULL;
}

static int minix_file_write(struct file *file,
			char* buf, int count,int offset)
{
	return 0;
}

static int minix_file_read(struct file *file,
			char* buf, int count,int offset)
{
	int left,chars,nr;
	struct buffer_head * bh;
	struct inode *inode;
	inode = file->f_inode;

	
	if ((left = count) <= 0)
		return 0;

	while (left) {
		nr = _bmap(inode,(file->f_pos)/BLOCK_SIZE);
		if (nr) {
			bh = buffer_read(inode->i_dev,nr);
			if (!bh)
				break;
		} else
			bh = NULL;

		nr = file->f_pos % BLOCK_SIZE;
		chars = MIN( BLOCK_SIZE-nr , left );

		file->f_pos += chars;
		left -= chars;

		if (bh) {
			char *p = nr + bh->b_data;
			while (chars-->0)
				put_fs_byte(*(p++),buf++);
			//brelse(bh);
		} else {
			while (chars-->0)
				put_fs_byte(0,buf++);
		}

	}
	return (count-left)?(count-left):-ERROR;
}

static int minix_file_open(struct inode *inode,struct file *file)
{
	printk("minix opened!\n");
	return 0;
}

static int _bmap(struct inode *inode,int block)
{
	struct minix1_inode *m;

	if (inode && inode->i_inode)
		m = inode->i_inode;
	else
		panic("inode!");

	if (block<0)
		panic("_bmap: block<0");
	if (block >= 7)
		panic("_bmap: block>big");

	if (block<7) {
		return m->i_zone[block];
	}
	return 0;
}





