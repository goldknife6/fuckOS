#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>
int write(int fd,char *buf,int count)
{
	struct file *file;
	struct inode * inode;
	int retval;
	retval = find_file(fd,&file,0);
	if (retval < 0)
		return retval;
	if (file->f_op->write)
		return file->f_op->write(file,buf,count,0);
	else
		return -1;	
}
