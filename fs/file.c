#include <fuckOS/fs.h>
#include <fuckOS/task.h>

#include <mm/slab.h>
#include <string.h>
/*
struct file {
	uint32_t f_mode;
	uint32_t f_flags;
	uint32_t f_count;
	struct inode *f_inode;
	uint32_t f_pos;
};
*/
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

	f = kmalloc(sizeof(struct file));
	if (!f)
		return -ENOMEM;
	
	memset(f,0,sizeof(struct file));
	*file = f;
	curtask->files->fd[fd] = f;
	return fd;
}
int find_file(int fd,struct file **file,int mode)
{
	if (curtask->files->fd[fd]) {
		*file = curtask->files->fd[fd];
	} else {
		*file = NULL;
		return -ENOENT;
	}
	return 0;
}
