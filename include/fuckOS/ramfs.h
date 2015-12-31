#ifndef _MINIOS_RAMFS_H
#define _MINIOS_RAMFS_H
#include <types.h>

#include <fuckOS/fs.h>
#include <errno.h>

#define RAMFS_MAGIC 0x1


static struct dentry *simple_lookup(struct inode *dir, 
			struct dentry *dentry,struct nameidata* nd)
{
	return NULL;
}


extern struct inode *ramfs_get_inode(struct super_block *,int , int );
#endif
