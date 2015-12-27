#include <fuckOS/fs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>


static void read_inode(struct m_inode *);
static int _bmap(struct m_inode *,int ,int );

struct m_inode *alloc_inode()
{
	struct m_inode *inode;
	
	inode = kmalloc(sizeof(struct m_inode));
	if (!inode) 
		return NULL;
	memset(inode,0,sizeof(struct m_inode));


	return inode;
}

struct m_inode *inode_get(int dev,int nr)
{
	struct m_inode *inode = alloc_inode();

	if (!inode)
		return NULL;

	inode->i_dev = dev;
	inode->i_num = nr;

	read_inode(inode);

	return inode;
}

static void read_inode(struct m_inode * inode)
{
	struct super_block * sb;
	struct buffer_head * bh;
	int block;

	if (!(sb = get_super(inode->i_dev))) {
		panic("trying to read inode without dev :%x\n",inode->i_dev);
	}

	block = 2 + sb->s_imap_blocks + sb->s_zmap_blocks +
		(inode->i_num)/INODES_PER_BLOCK;

	if (!(bh=buffer_read(inode->i_dev,block)))
		panic("unable to read i-node block");

	*(struct d_inode *)inode =
		((struct d_inode *)bh->b_data)
			[(inode->i_num)%INODES_PER_BLOCK];
	//brelse(bh);
	//unlock_inode(inode);
}
static int _bmap(struct m_inode * inode,int block,int create)
{
	struct buffer_head * bh;
	int i;

	if (block<0)
		panic("_bmap: block<0");
	//if (block >= 7+512+512*512)
	if (block >= 7)
		panic("_bmap: block>= 7");
	if (block<7) {
		if (create && !inode->i_zone[block]) {
			if ((inode->i_zone[block] = new_block(inode->i_dev))) {
				//inode->i_ctime=CURRENT_TIME;
				inode->i_dirt=1;
			}
		}
		return inode->i_zone[block];
	}
	panic("not implement\n");
	return 0;
}

int bmap(struct m_inode * inode,int block)
{
	return _bmap(inode,block,0);
}
