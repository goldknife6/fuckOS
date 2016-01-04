#include <fuckOS/libfs.h>
#include <fuckOS/assert.h>
#include <sys/stat.h>

#include <mm/slab.h>

#include <string.h>



struct file_operations simple_dir_operations = 
{
	.read = simple_dir_read
};


int simple_dir_read(struct file *file ,
		char* buf, int count,int offset)
{
	panic("simple_dir_read not implemented!\n");
	return 0;
}









struct file_operations simple_file_operations = 
{
	.read = simple_file_read
};

int simple_file_read(struct file *file ,
		char* buf, int count,int offset)
{
	panic("simple_file_read not implemented!\n");
	return 0;
}











struct inode_operations simple_dir_inode_operations = 
{
	.lookup = simple_dir_inode_lookup,
	.create = simple_dir_inode_create,
	.mkdir = simple_dir_inode_mkdir,
	.rmdir = simple_dir_inode_rmdir,
	.mknod = simple_dir_inode_mknod,
	.rename = simple_dir_inode_rename,
};

struct dentry * 
simple_dir_inode_lookup(struct inode *inode
		,struct dentry *dentry, struct nameidata* nd)
{
	return NULL;
}

int 
simple_dir_inode_create(struct inode *dir
			,struct dentry *dentry, 
			int mode,struct nameidata* nd)
{
	return simple_dir_inode_mknod(dir, dentry, 
					mode | S_IFREG, 0);
}

int 
simple_dir_inode_mkdir(struct inode *dir
			,struct dentry *dentry,int mode)
{
	int retval = simple_dir_inode_mknod(dir, dentry, 
					mode | S_IFDIR, 0);  
	if (!retval)  
		dir->i_nlink++;
	return retval; 
}

int 
simple_dir_inode_rmdir(struct inode *inode
				,struct dentry *dentry)
{
	panic("simple_dir_inode_rmdir!\n");
	return 0;
}

int 
simple_dir_inode_mknod(struct inode *dir
			,struct dentry *dentry,
				int mode,int dev)
{
	struct inode * inode = simple_get_inode(dir->i_sb, mode, dev);  
	int error = -ENOSPC;  

	if (inode) {  
		dentry->d_inode = inode;
		atomic_set(&dentry->d_count,1);
		error = 0;  
	}  
	return error; 
}
int 
simple_dir_inode_rename(struct inode *iscr, 
			struct dentry *dscr,
			struct inode *ides, 
			struct dentry *ddes)
{
	return 0;
}













struct inode_operations simple_file_inode_operations = 
{
	.lookup = simple_file_inode_lookup,
	.create = simple_file_inode_create,
	.mkdir = simple_file_inode_mkdir,
	.rmdir = simple_file_inode_rmdir,
	.mknod = simple_file_inode_mknod,
	.rename = simple_file_inode_rename,
};

struct dentry * 
simple_file_inode_lookup(struct inode *inode
		,struct dentry *dentry, struct nameidata* nd)
{
	panic("simple_file_inode_create lookup\n");
	return NULL;
}

int 
simple_file_inode_create(struct inode *inode
			,struct dentry *dentry, 
			int mode,struct nameidata* nd)
{
	panic("simple_file_inode_create called\n");
	return 0;
}

int 
simple_file_inode_mkdir(struct inode *inode
			,struct dentry *dentry,int mode)
{
	panic("simple_file_inode_mkdir called\n");
	return 0;
}
int 
simple_file_inode_rmdir(struct inode *inode
				,struct dentry *dentry)
{
	panic("simple_file_inode_rmdir called\n");
	return 0;
}

int 
simple_file_inode_mknod(struct inode *inode
			,struct dentry *dentry,
				int flags,int mode)
{
	panic("simple_file_inode_mknod called\n");
	return 0;
}
int 
simple_file_inode_rename(struct inode *iscr, 
			struct dentry *dscr,
			struct inode *ides, 
			struct dentry *ddes)
{
	panic("simple_file_inode_rename called\n");
	return 0;
}












struct super_operations simple_super_operations = 
{
	.alloc_inode = simple_super_alloc_inode,
	.destroy_inode = simple_super_destroy_inode,
	.read_inode = simple_super_read_inode,
	.read_super = simple_super_read_super,
};

struct inode *
simple_super_alloc_inode(struct super_block *sb)
{
	panic("simple_super_alloc_inode called\n");
	return NULL;
}

void
simple_super_destroy_inode(struct inode *inode)
{
	if (inode)
		kfree(inode);
	panic("simple_super_destroy_inode called\n");
}

int simple_super_read_inode(struct inode *inode)
{
	panic("simple_super_read_inode called\n");
	return 0;
}
int simple_super_read_super(struct super_block * sb, 
				void *data, int slient)
{
	panic("simple_super_read_super called\n");
	return 0;
}

struct inode *
simple_get_inode(struct super_block *sb,
				int mode, int dev)
{
	struct inode *inode = new_inode(sb);
	
	if (inode) {
		switch (mode & S_IFMT) {
		case S_IFREG:  
			inode->i_op = &simple_file_inode_operations;  
			inode->i_fop = &simple_file_operations;  
			break;
		case S_IFDIR:  
			inode->i_op = &simple_dir_inode_operations;  
			inode->i_fop = &simple_dir_operations; 
			break;
		}
		inode->i_mode = mode;
		inode->i_sb = sb;
		
	}
	return inode;
}

struct inode * create_node(struct dentry *parent, 
			struct qstr *name,int mode,
				struct file_operations *fop,
				struct inode_operations *iop)
{
	struct inode *inode = NULL;
	struct dentry *dentry;
	struct super_block *sb;
	if (parent) {
		sb = parent->d_sb;
		inode = simple_get_inode(sb, mode, sb->s_dev);
		assert(inode);
		if (iop)
			inode->i_op = iop;
		if (fop)
			inode->i_fop = fop;
		dentry = d_alloc(parent, name);
		assert(dentry);
		dentry->d_inode = inode;
		dentry_insert(parent,dentry);
		printk("create node:%.*s %o\n",name->len,name->name,mode);
	}
	return inode;
}

