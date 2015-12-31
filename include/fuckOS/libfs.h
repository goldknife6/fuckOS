#ifndef _MINIOS_LIBFS_H
#define _MINIOS_LIBFS_H
#include <fuckOS/fs.h>

extern struct file_operations simple_dir_operations;
extern struct file_operations simple_file_operations;


extern struct inode_operations simple_dir_inode_operations;
extern struct inode_operations simple_file_inode_operations;

extern struct super_operations simple_super_operations;
#endif/*!_MINIOS_LIBFS_H*/
