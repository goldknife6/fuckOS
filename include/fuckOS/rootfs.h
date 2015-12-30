#ifndef _MINIOS_ROOTFS_H
#define _MINIOS_ROOTFS_H

#include <fuckOS/fs.h>

extern struct super_operations root_super_op;
extern struct inode_operations root_inode_op;
#endif/*!_MINIOS_ROOTFS_H*/
