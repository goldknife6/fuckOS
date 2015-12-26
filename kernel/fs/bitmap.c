#include <fuckOS/fs.h>
#include <fuckOS/assert.h>

void free_block(int dev, int block)
{
	struct super_block * sb;
	struct buffer_head * bh;

}

int new_block(int dev)
{
	return 0;
}

void free_inode(struct m_inode * inode)
{
	
}

struct m_inode * new_inode(int dev)
{
	struct m_inode * inode = NULL;
	
	return inode;
}
