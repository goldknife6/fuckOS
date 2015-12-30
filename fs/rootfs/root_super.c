#include <fuckOS/rootfs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int root_read_super(struct super_block *);

struct super_operations root_super_op = 
{
	.read_super = root_read_super,
};


int root_read_super(struct super_block *sb)
{
	struct inode *inode = NULL;
	inode = alloc_inode(&root_inode_op,sb,0,sb->s_dev,NULL,S_IFDIR);
	assert(inode);
	return 0;
}
