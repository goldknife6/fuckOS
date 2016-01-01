#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>

static struct file *_open(struct dentry *,int,int,struct vfsmount *);

int open(char *pathname,int len,int flags,int mode)
{
	struct inode *inode;
	struct dentry *parent,*d;
	struct file *file;
	int retval,fd = -1;
	struct nameidata nd;

	fd = get_unused_fd();
	if (fd < 0)
		return fd;

	retval = path_lookup(pathname,0,LOOKUP_OPEN|LOOKUP_FOLLOW,&nd);

	if (retval < 0)
		return retval;
	
	file = _open(nd.dentry,mode,flags,nd.mnt);
	
	if (file)
		curtask->files->fd[fd] = file;
	else
		fd = -1;

	return fd;
}


static struct file * 
_open(struct dentry *dentry,int mode,int flags,struct vfsmount *mnt)
{
	struct file *file = alloc_file_struct();
	struct inode *inode = dentry->d_inode;
	int res;
	if (file) {
		file->f_mode = mode;
		file->f_flags = flags;
		file->f_dentry = dentry;
		file->f_vfsmnt = mnt;
		file->f_op = inode->i_fop;
		if (file->f_op->open) {
			res = file->f_op->open(inode,file);
		}
	}
	return file;
}

