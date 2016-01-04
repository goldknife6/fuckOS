#ifndef _MINIOS_RAMFS_H
#define _MINIOS_RAMFS_H
#include <types.h>

#include <fuckOS/fs.h>
#include <errno.h>

#define RAMFS_MAGIC 0x1

struct ram_file {
	int size;
	uint8_t *start;
};
extern struct file_operations ramfs_file_operations;
extern struct inode_operations ramfs_dir_inode_operations;
extern struct inode *ramfs_get_inode(struct super_block *,int , int );


#endif
