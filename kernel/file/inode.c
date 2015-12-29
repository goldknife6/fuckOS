#include <fuckOS/fs.h>
#include <drivers/hd.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>

#define MAX_HASH_INODE	1024
#define inode_hashfn(dev,num) (((uint32_t)((dev)^(num)))%MAX_HASH_INODE)
#define inode_hash(dev,num) inode_table[inode_hashfn(dev,num)]


static struct dentry* find_dentry(struct dentry *,char *,int);



struct hlist_head inode_table[MAX_HASH_INODE];

struct inode *alloc_inode(struct inode_operations *op,
			struct super_block *sb,int num,
			int dev,void *i)
{
	struct inode *inode;
	inode = kmalloc(sizeof(struct inode));

	if (!inode)
		return NULL;

	memset(inode,0,sizeof(struct inode));

	INIT_HLIST_NODE(&inode->i_hash);
	inode->i_op = op;
	inode->i_sb = sb;
	inode->i_num = num;
	inode->i_dev = dev;
	inode->i_inode = i;
	return inode;
}

struct inode *find_inode(int num,int dev)
{
	struct inode *inode = NULL;
	struct hlist_head* inode_head;
	inode_head = &inode_hash(dev,num);

	hlist_for_each_entry(inode, inode_head, i_hash) {
		if (inode->i_num == num && inode->i_dev == dev)
			return inode;
	}

	return NULL;
}

void insert_inode(struct inode *inode)
{
	struct hlist_head* inode_head;
	inode_head = &inode_hash(inode->i_dev,inode->i_num);
	hlist_add_head(&inode->i_hash, inode_head);
}

void delete_inode(struct inode *inode)
{
	struct hlist_head* inode_head;
	struct inode *tmp;
	tmp = find_inode(inode->i_dev,inode->i_num);
	assert(tmp);
	assert(tmp == inode);
	hlist_del(&inode->i_hash);
}

struct dentry* path_lookup(char *pathname,int namelen)
{
	char* thisname;
	char c;
	struct dentry *root, *dir;
	struct inode *inode;
	int len;

	if (namelen <= 0)
		return NULL;
	
	if (!curtask->fs->root)
		panic("No root inode");
	if (!curtask->fs->pwd)
		panic("No cwd inode");

	root = curtask->fs->pwd;
	//inode = (struct inode *)root->d_inode;

	if (pathname[0] == '/')
		root = curtask->fs->root;
	dir = root;
	while (1) {
		thisname = pathname;

		for(len=0; (c = *(pathname++)) && (c != '/'); len++)
			/* nothing */ ;

		if (!c)
			return dir;

		dir = find_dentry(dir,thisname,len);
		if (!dir)
			return NULL;
	}


	return NULL;
}

static struct dentry* 
find_dentry(struct dentry *parent,
			char *name,int len)
{
	struct dentry *dir;
	struct inode *inode;
	dir = lookup_dentry(parent,name,len);
	if (dir)
		return dir;
	inode = parent->d_inode;
	
	return inode->i_op->lookup(name,parent,len);
}
