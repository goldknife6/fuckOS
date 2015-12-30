#include <fuckOS/fs.h>
#include <fuckOS/assert.h>

#include <sys/stat.h>

#include <string.h>
#include <errno.h>

static struct super_operations ramfs_ops;
static struct inode_operations ramfs_file_inode_operations;


static int ramfs_create(struct inode *,struct dentry *, int , struct nameidata *) ;
static int ramfs_mknod(struct inode *, struct dentry *, int , int);
static int ramfs_mkdir(struct inode *, struct dentry * , int);

static struct inode_operations ramfs_dir_inode_operations = 
{  
    .create     = ramfs_create,  
    //.lookup     = simple_lookup,  
    .mkdir      = ramfs_mkdir,  
    //.rmdir      = simple_rmdir,  
    .mknod      = ramfs_mknod,  
    //.rename     = simple_rename,  
};  

struct inode *
ramfs_get_inode(struct super_block *sb,
				int mode, int dev)
{
	struct inode *inode = new_inode(sb);
	
	if (inode) {
		inode->i_mode = mode;

		switch (mode & S_IFMT) {
		case S_IFREG:  
			//inode->i_op = &ramfs_file_inode_operations;  
			//inode->i_fop = &ramfs_file_operations;  
			break;
		case S_IFDIR:  
			inode->i_op = &ramfs_dir_inode_operations;  
			//inode->i_fop = &simple_dir_operations; 
			break;
		}
		inode->i_sb = sb;
	}
	return inode;
}

static int  
ramfs_mknod(struct inode *dir, 
		struct dentry *dentry, int mode, int dev)  
{  
	struct inode * inode = ramfs_get_inode(dir->i_sb, mode, dev);  
	int error = -ENOSPC;  

	if (inode) {  
		dentry->d_inode = inode;
		//dentry->d_count++;
		error = 0;  
	}  
	return error;  
}

static int 
ramfs_create(struct inode *dir, 
		struct dentry *dentry, int mode, 
				struct nameidata *nd)  
{  
	return ramfs_mknod(dir, dentry, mode | S_IFREG, 0);  
} 

static int 
ramfs_mkdir(struct inode *dir, 
			struct dentry * dentry, int mode)  
{  
    int retval = ramfs_mknod(dir, dentry, mode | S_IFDIR, 0);  
    if (!retval)  
        dir->i_nlink++;
    return retval;  
} 
