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

int dup2(int oldfd,int newfd)
{
	struct file *file,*old;
	struct inode * inode;
	int retval,fd;

	if (oldfd == newfd)
		return 0;

	retval = find_file(oldfd,&old,0);
	if (retval < 0)
		return retval;

	if (curtask->files->fd[newfd])
		return -1;

	curtask->files->fd[newfd] = old;
	old->f_count++;

	return 0;
}
