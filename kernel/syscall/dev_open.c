#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>

static int file_fd(struct inode *);


int open(char *filename,int flag,int mode)
{
	struct inode *inode;
	struct file * f;
	int retval,fd = -1;
	struct nameidata nd;

	nd.flags = flag;
	nd.dentry = NULL;
	nd.mnt = NULL;

	retval = path_lookup(filename,flag,&nd);

	printk("user call retval:%d\n",retval);
	return fd;
}

static int file_fd(struct inode *inode)
{
	int retval,fd = -1,i;
	
	return fd;
}
