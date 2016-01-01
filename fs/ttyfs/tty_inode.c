#include <fuckOS/fs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

static int ttyfs_file_read(struct file *,char*,int,int);
static int ttyfs_file_write(struct file *,char*,int,int);
static int ttyfs_file_open(struct inode *,struct file *);

struct inode_operations ttyfs_inode_op = 
{
	.lookup = NULL,
};

struct file_operations ttyfs_file_op = 
{
	.read = ttyfs_file_read,
	.open = ttyfs_file_open,
	.write = ttyfs_file_write,
};


static int ttyfs_file_write(struct file *file,
			char* buf, int count,int offset)
{
	printk("%.*s",count,buf);
	return 0;
}

static int ttyfs_file_read(struct file *file,
			char* buf, int count,int offset)
{
	return 0;
}

static int ttyfs_file_open(struct inode *inode,struct file *file)
{
	printk("tty opened!\n");
	return 0;
}

	
	
