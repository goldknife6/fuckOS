#ifndef _MINIOS_ROOTFS_H
#define _MINIOS_ROOTFS_H

#include <fuckOS/fs.h>


#define ROOTFS_MAGIC 0x2

extern struct super_operations root_super_op;
extern struct inode_operations root_inode_op;
extern struct file_operations rootfs_file_op;
#endif/*!_MINIOS_ROOTFS_H*/
