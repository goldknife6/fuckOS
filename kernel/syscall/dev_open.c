#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>

static int file_fd(struct inode *);

int open(char * filename,int flag,int mode)
{
	struct inode *inode;
	struct file * f;
	int retval,fd = -1;



	return fd;
}

static int file_fd(struct inode *inode)
{
	int retval,fd = -1,i;
	
	return fd;
}
