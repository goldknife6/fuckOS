#include <fuckOS/task.h>
#include <fuckOS/pipe.h>

#include <mm/slab.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

static struct inode *get_pipe_inode(void);
static struct pipe_inode_info *get_pipe_info(void);
static int pipe_new(struct inode *inode);


int pipe(int fd[2],int flags)
{
	int error,fd1,fd2;
	struct file *f1,*f2;
	struct inode *inode;
	struct dentry *dentry;
	struct qstr name;

	error = -ENFILE;
	fd1 = get_empty_file(&f1);
	if (fd1 < 0)
		goto ret;

	fd2 = get_empty_file(&f2);
	if (fd2 < 0)
		goto ret;

	inode = get_pipe_inode();

	if (!inode)
		goto ret;


	error = -ENOMEM;
	dentry = d_alloc(pipe_mnt->mnt_sb->s_root, &name);
	if (!dentry) {
		goto ret;
	}

	f1->f_dentry = f2->f_dentry = dentry;
	f1->f_vfsmnt = f2->f_vfsmnt = pipe_mnt;
	f1->f_inode = f2->f_inode = inode;
	f1->f_count = f2->f_count = 1;

	f1->f_pos = f2->f_pos = 0;
	f1->f_flags = O_RDONLY;
	f1->f_op = &pipefs_file_op;

	f2->f_flags = O_WRONLY;
	f2->f_op = &pipefs_file_op;

	fd[0] = fd1;
	fd[1] = fd2;
	curtask->files->fd[fd1] = f1;
	curtask->files->fd[fd2] = f2;
	return 0;

ret:
	return error;
}


static struct inode *get_pipe_inode(void)
{
	struct inode *inode = new_inode(pipe_mnt->mnt_sb);
	if (inode) {
		if (!pipe_new(inode)) {
			inode->i_fop = &pipefs_file_op;	
			inode->i_mode = S_IFIFO;
		} else {
			kfree(inode);
			inode = NULL;		
		}
	}
	return inode;
}

static struct pipe_inode_info *
get_pipe_info(void)
{
	struct pipe_inode_info *info;
	info = kmalloc(sizeof(struct pipe_inode_info));
	if (info) {
		struct page *new;
		new = page_alloc(_GFP_ZERO);
		if (!new) {
			kfree(info);
			return NULL;		
		}	
		info->p_page = new;
		info->p_rpos = 0;
		info->p_wpos = 0;
		info->p_reader = 1;
		info->p_writer = 1;
	}
	return info;
}

static int pipe_new(struct inode *inode)
{
	struct pipe_inode_info *info;
	info = get_pipe_info();
	if (!info)
		return -ENOMEM;
	inode->i_pipe = info;
	inode->i_inode = NULL;
	return 0;
}
