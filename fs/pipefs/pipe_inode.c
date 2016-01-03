#include <fuckOS/pipe.h>
#include <fuckOS/assert.h>
#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>

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
	struct inode *inode;
	struct pipe_inode_info *info;
	int i;
	char *start;

	inode = file->f_inode;
	info = (struct pipe_inode_info *)inode->i_pipe;

	start = (char*)page2virt(info->p_page);


	for (i = 0; i < count; i++) {
		if (info->p_wpos >= info->p_rpos + PAGE_SIZE) {
			return i;
		}

		start[info->p_wpos % PAGE_SIZE] = buf[i];
		info->p_wpos++;
	}

	return i;
}

static int pipefs_file_read(struct file *file,
			char* buf, int count,int offset)
{
	struct inode *inode;
	struct pipe_inode_info *info;
	char *start;
	int i;

	inode = file->f_inode;
	info = (struct pipe_inode_info *)inode->i_pipe;
	start = (char*)page2virt(info->p_page);



	for (i = 0; i < count; i++) {
		if (info->p_rpos == info->p_wpos) {
			return i;
		}
		buf[i] = start[info->p_rpos % PAGE_SIZE];
		info->p_rpos++;
	}

	return i;
}

static int pipefs_file_open(struct inode *inode,struct file *file)
{
	printk("pipe opened!\n");
	return 0;
}


