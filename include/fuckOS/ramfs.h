#ifndef _MINIOS_RAMFS_H
#define _MINIOS_RAMFS_H
#include <types.h>

#include <fuckOS/fs.h>

#define RAMFS_MAGIC 0x1



extern struct inode *ramfs_get_inode(struct super_block *,int , int );
#endif
