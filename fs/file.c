#include <fuckOS/fs.h>
#include <fuckOS/task.h>
#include <mm/slab.h>
#include <string.h>

int get_empty_file(struct file **file)
{
	int i,fd = -1;
	struct file *f;
	for (i = 0;i < MAX_FILE; i++) {
		if (!curtask->files->fd[i]) {
			fd = i;
			break;
		}
	}
	if (fd < 0)
		return -EMFILE;

	f = alloc_file();
	if (!f)
		return -ENOMEM;
	*file = f;
	curtask->files->fd[fd] = f;
	return fd;
}

int get_unused_fd()
{
	int i,fd = -1;
	for (fd = 0;fd < MAX_FILE; fd++) {
		if (!curtask->files->fd[fd]) {
			break;
		}
	}
	if (fd >= MAX_FILE)
		return -EMFILE;
	else
		return fd;
}

int find_file(int fd,struct file **file,int mode)
{
	if (curtask->files->fd[fd]) {
		*file = curtask->files->fd[fd];
	} else {
		*file = NULL;
		return -EBADF;
	}
	return 0;
}

void deref_file(int fd)
{
	struct file *file = curtask->files->fd[fd];
	if (!file)
		panic("deref_file\n");
	
	if (file->f_op && file->f_op->flush)
		file->f_op->flush(file);

	file->f_count--;
	if (!file->f_count) {
		if (file->f_op && file->f_op->release)
			file->f_op->release(NULL,file);
		free_file(file);
	}
	curtask->files->fd[fd] = NULL;
}

void free_file(struct file *file)
{
	if (file)
		kfree(file);
}


struct file *alloc_file()
{
	struct file *file;

	file = kmalloc(sizeof(struct file));
	if (!file)
		return NULL;
	memset(file,0,sizeof(struct file));
	return file;
}
