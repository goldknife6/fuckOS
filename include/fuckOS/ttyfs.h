#ifndef _MINIOS_TTYFS_H
#define _MINIOS_TTYFS_H
#include <types.h>

#include <fuckOS/fs.h>
#include <fuckOS/libfs.h>
#include <errno.h>

#define TTYFS_MAGIC 0x3


extern struct super_operations ttyfs_super_op;
extern struct inode *ttyfs_get_inode(struct super_block *,int , int );
extern struct inode_operations ttyfs_inode_op;
extern struct file_operations ttyfs_file_op;
#endif
