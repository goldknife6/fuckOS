#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

#include "pipe.h"
static int pipefs_file_read(struct file *,char*,int,int);
static int pipefs_file_write(struct file *,char*,int,int);
static int pipefs_file_open(struct inode *,struct file *);
static int pipe_new(struct inode *);
static struct pipe_inode_info *get_pipe_info(void);

struct inode_operations pipefs_inode_op = 
{
	.lookup = NULL,
};

struct file_operations pipefs_file_op = 
{
	.read = pipefs_file_read,
	.open = pipefs_file_open,
	.write = pipefs_file_write,
};


static int pipefs_file_write(struct file *file,
			char* buf, int count,int offset)
{
	return 0;
}

static int pipefs_file_read(struct file *file,
			char* buf, int count,int offset)
{
	return 0;
}

static int pipefs_file_open(struct inode *inode,struct file *file)
{
	printk("pipe opened!\n");
	return 0;
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
	
	
