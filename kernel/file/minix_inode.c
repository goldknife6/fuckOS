#include <fuckOS/minixfs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>



struct inode_operations minix_inode_op = 
{
	
};

void print_minix_inode(struct inode* inode)
{
	assert(inode);
	assert(inode->i_inode);

	struct minix1_inode *i = (struct minix1_inode *)inode->i_inode;

	printk("size:%d zone[0]:%d time:%d\n",i->i_size,i->i_zone[0],i->i_time);
}
struct inode* get_minix_inode(int num,struct super_block *sb)
{
	struct inode *inode;
	struct minix1_inode* i;

	inode = find_inode(num,sb->s_dev);
	if (inode)
		return inode;
	
	i = kmalloc(sizeof(struct minix1_inode));
	if (!i)
		return NULL;
	memset(i,0,sizeof(struct minix1_inode));
	
	inode = get_inode(&minix_inode_op,sb,num,sb->s_dev,(void *)i);

	if (!inode) {
		kfree(i);
		return NULL;
	}

	read_inode(sb,inode);

	insert_inode(inode);
	return inode;
}












