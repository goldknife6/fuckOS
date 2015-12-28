#include <fuckOS/fs.h>
#include <drivers/hd.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>

#define MAX_HASH_INODE	1024
#define inode_hashfn(dev,num) (((uint32_t)((dev)^(num)))%MAX_HASH_INODE)
#define inode_hash(dev,num) inode_table[inode_hashfn(dev,num)]




struct hlist_head inode_table[MAX_HASH_INODE];

struct inode *get_inode(struct inode_operations *op,
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

