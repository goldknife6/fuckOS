#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>

static int file_fd(struct m_inode *);

int open(char * filename,int flag,int mode)
{
	struct m_inode *inode;
	struct file * f;
	int retval,fd = -1;

	retval = open_namei(filename, flag, mode, &inode);
	
	if (retval < 0)
		return retval;
	
	fd = file_fd(inode);


	return fd;
}

static int file_fd(struct m_inode *inode)
{
	int retval,fd = -1,i;
	struct file *file;
	for (i = 0; i < MAX_FILES; i++) {
		if (!curtask->files->fd[i]) {
			file = kmalloc(sizeof(struct file));
			curtask->files->fd[i] = file;
			assert(file);
			fd = i;
			file->f_inode = inode;
			file->f_pos = 0;
			break;
		}
	}
	return fd;
}
