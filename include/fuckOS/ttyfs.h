#ifndef _MINIOS_TTYFS_H
#define _MINIOS_TTYFS_H
#include <types.h>

#include <fuckOS/fs.h>
#include <errno.h>

#define TTYFS_MAGIC 0x3



extern struct inode *ttyfs_get_inode(struct super_block *,int , int );
#endif
