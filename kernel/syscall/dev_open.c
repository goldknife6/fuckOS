#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>

static int file_fd(struct inode *);

/*
struct file {
	uint32_t f_mode;
	uint32_t f_flags;
	uint32_t f_count;
	struct inode *f_inode;
	uint32_t f_pos;
};
*/

int open(char *filename,int flag,int mode)
{
	struct inode *inode;
	struct file *f;
	int retval,fd = -1;
	struct nameidata nd;

	nd.flags = flag;
	nd.dentry = NULL;
	nd.mnt = NULL;

	retval = path_lookup(filename,strlen(filename),flag,&nd);
	if (retval < 0)
		return -1;

	inode = nd.dentry->d_inode;
	if (inode->i_mode)
	fd = get_empty_file(&f);

	f->f_mode = mode;
	f->f_flags = flag;
	f->f_count = 1;
	f->f_inode = inode;
	f->f_pos = 0;
	return fd;
}
static int _open(struct inode *inode)
{
	return 0;
}
static int file_fd(struct inode *inode)
{
	int retval,fd = -1,i;
	
	return fd;
}
