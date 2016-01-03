#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <errno.h>

int dup(int odlfd)
{
	struct file *file,*old;
	struct inode * inode;
	int retval,fd;
	retval = find_file(odlfd,&old,0);
	if (retval < 0)
		return retval;

	fd = get_unused_fd();
	if (fd < 0)
		return fd;

	curtask->files->fd[fd] = old;
	old->f_count++;

	return fd;
}
