#ifndef _MINIOS_PIPE_H
#define _MINIOS_PIPE_H
#include <types.h>
#include <mm/pages.h>
#include <fuckOS/fs.h>
#include <sys/stat.h>
#define PIPEFS_MAGIC 4


struct pipe_inode_info {
	struct page *p_page;
	uint32_t p_rpos;
	uint32_t p_wpos;
	int	p_reader;
	int	p_writer;
};



extern struct super_operations pipefs_super_op;
extern struct vfsmount *pipe_mnt;
extern struct inode_operations pipefs_inode_op;
extern struct file_operations pipefs_file_op;
#endif/*!_MINIOS_PIPE_H*/
