#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>

int fdtype(int fd)
{
	struct file *old;
	struct inode * inode;
	int retval;
	retval = find_file(fd,&old,0);
	if (retval < 0)
		return retval;
	inode = old->f_inode;
	
	return ((inode->i_mode) & S_IFMT);
}

