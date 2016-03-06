#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>

void close(int fd)
{
	struct file *file;
	struct inode * inode;
	int retval;
	retval = find_file(fd,&file,0);
	if (retval < 0)
		return;
	
	//deref_file(fd);	
}
