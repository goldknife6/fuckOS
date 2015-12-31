#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <sys/stat.h>

#include <mm/slab.h>

#include <string.h>


static int simple_dir_read(struct file * ,char* , int , int);
static int simple_file_read(struct file * ,char* , int , int);

static struct dentry * simple_dir_inode_lookup(struct inode *,struct dentry *, struct nameidata* );
static int simple_dir_inode_create(struct inode *,struct dentry *, int ,struct nameidata* );
static int simple_dir_inode_mkdir(struct inode *,struct dentry *,int );
static int simple_dir_inode_rmdir(struct inode *,struct dentry *);
static int simple_dir_inode_mknod(struct inode *,struct dentry *,int ,int );
static int simple_dir_inode_rename(struct inode *, struct dentry *,struct inode *, struct dentry *);


static struct dentry *simple_file_inode_lookup(struct inode *,struct dentry *, struct nameidata* );
static int simple_file_inode_create(struct inode *,struct dentry *, int ,struct nameidata* );
static int simple_file_inode_mkdir(struct inode *,struct dentry *,int );
static int simple_file_inode_rmdir(struct inode *,struct dentry *);
static int simple_file_inode_mknod(struct inode *,struct dentry *,int ,int );
static int simple_file_inode_rename(struct inode *, struct dentry *,struct inode *, struct dentry *);


static struct inode *simple_super_alloc_inode(struct super_block *);
static void simple_super_destroy_inode(struct inode *);
static int simple_super_read_inode(struct inode *);
static int simple_super_read_super(struct super_block * , void *, int );

struct file_operations simple_dir_operations = 
{
	.read = simple_dir_read
};


static int simple_dir_read(struct file *file ,
		char* buf, int count,int offset)
{
	panic("simple_dir_read not implemented!\n");
	return 0;
}









struct file_operations simple_file_operations = 
{
	.read = simple_file_read
};

static int simple_file_read(struct file *file ,
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

static struct dentry * 
simple_dir_inode_lookup(struct inode *inode
		,struct dentry *dentry, struct nameidata* nd)
{
	return NULL;
}

static int 
simple_dir_inode_create(struct inode *inode
			,struct dentry *dentry, 
			int mode,struct nameidata* nd)
{
	return 0;
}

static int 
simple_dir_inode_mkdir(struct inode *inode
			,struct dentry *dentry,int mode)
{
	return 0;
}
static int 
simple_dir_inode_rmdir(struct inode *inode
				,struct dentry *dentry)
{
	return 0;
}

static int 
simple_dir_inode_mknod(struct inode *inode
			,struct dentry *dentry,
				int flags,int mode)
{
	return 0;
}
static int 
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

static struct dentry * 
simple_file_inode_lookup(struct inode *inode
		,struct dentry *dentry, struct nameidata* nd)
{
	panic("simple_file_inode_create lookup\n");
	return NULL;
}

static int 
simple_file_inode_create(struct inode *inode
			,struct dentry *dentry, 
			int mode,struct nameidata* nd)
{
	panic("simple_file_inode_create called\n");
	return 0;
}

static int 
simple_file_inode_mkdir(struct inode *inode
			,struct dentry *dentry,int mode)
{
	panic("simple_file_inode_mkdir called\n");
	return 0;
}
static int 
simple_file_inode_rmdir(struct inode *inode
				,struct dentry *dentry)
{
	panic("simple_file_inode_rmdir called\n");
	return 0;
}

static int 
simple_file_inode_mknod(struct inode *inode
			,struct dentry *dentry,
				int flags,int mode)
{
	panic("simple_file_inode_mknod called\n");
	return 0;
}
static int 
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

static struct inode *
simple_super_alloc_inode(struct super_block *sb)
{
	panic("simple_super_alloc_inode called\n");
	return NULL;
}

static void
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
