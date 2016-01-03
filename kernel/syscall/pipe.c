#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/fs.h>

#include <mm/pages.h>
#include <mm/slab.h>
#include <errno.h>
#include <string.h>

int pipe(int fd[2],int flags)
{
	int error;
	struct file *f1,*f2;
	struct inode *inode;

	error = -ENFILE;
	error = get_empty_file(&f1);
	if (error < 0)
		goto ret;

	error = get_empty_file(&f2);
	if (error < 0)
		goto ret;


ret:
	return error;
}

