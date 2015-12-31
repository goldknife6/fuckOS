#include <fuckOS/fs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

static int ttyfs_file_write(struct file *,char*,int,int);

struct inode_operations ttyfs_inode_op = 
{
	
};

struct file_operations ttyfs_file_op = 
{
	.read = ttyfs_file_write
};


static int ttyfs_file_write(struct file *file,
			char* buf, int count,int offset)
{
	return 0;
}
