#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>

static int file_fd(struct inode *);


int open(char *pathname,int len,int flag,int mode)
{
	struct inode *inode;
	struct dentry *parent,*d;
	struct file *f;
	int retval,fd = -1;
	struct nameidata nd;


	retval = path_lookup(pathname,LOOKUP_OPEN|LOOKUP_FOLLOW,flag,&nd);

	if (retval < 0)
		return retval;

	inode = nd.dentry->d_inode;
		
	if (S_ISREG(inode->i_mode)) {
		fd = get_empty_file(&f);
		f->f_mode = mode;
		f->f_flags = flag;
		f->f_count = 1;
		f->f_inode = inode;
		f->f_pos = 0;
	} else {
		return -EISDIR;
	}
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
