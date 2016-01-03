#ifndef _MINIOS_PIPE_H
#define _MINIOS_PIPE_H
#include <types.h>
#include <mm/pages.h>


#define PIPEFS_MAGIC 4


struct pipe_inode_info {
	struct page *p_page;
	uint32_t p_rpos;
	uint32_t p_wpos;
};



extern struct super_operations pipefs_super_op;
extern struct vfsmount *pipe_mnt;

#endif/*!_MINIOS_PIPE_H*/
