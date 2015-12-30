#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>
extern struct file_operations rootfs_file_op;
int read(uint32_t fd,char *buf,int count)
{
	struct file *file;
	struct inode * inode;
	int retval;
	retval = find_file(fd,&file,0);
	if (retval < 0)
		return retval;
	
	inode = file->f_inode;

	return rootfs_file_op.read(file,buf,count,0);
}
