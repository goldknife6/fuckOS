#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <asm/bitops.h>


void free_block(int dev, int block)
{
	struct super_block * sb;
	struct buffer_head * bh;

}

int new_block(int dev)
{
	struct buffer_head * bh;
	struct super_block * sb;
	int i,j;

	if (!(sb = get_super(dev)))
		panic("trying to get new block from nonexistant device");

	j = 8192;
	for (i = 0 ; i < 8 ; i++) {
		if ((bh = sb->s_zmap[i])) {
			if ((j = find_first_zero(bh->b_data)) < 8192)
				break;
		}	
	}	

	if (i>=8 || !bh || j>=8192)
		return 0;

	if (test_and_set_bit(j,bh->b_data))
		panic("new_block: bit already set");

	bh->b_dirt = 1;

	j += i*8192 + sb->s_firstdatazone-1;

	if (j >= sb->s_nzones)
		return 0;

	if (!(bh= buffer_get(dev,j)))
		panic("new_block: cannot get block");

	if (bh->b_count != 1)
		panic("new block: count is != 1");

	//clear_block(bh->b_data);
	bh->b_uptodate = 1;
	bh->b_dirt = 1;
	//brelse(bh);
	return j;
}

void free_inode(struct inode * inode)
{
	
}

struct m_inode * new_inode(int dev)
{
	struct m_inode * inode;
	struct super_block * sb;
	struct buffer_head * bh;
	int i,j;

	inode = alloc_inode();
	if (!inode)
		return NULL;

	if (!(sb = get_super(dev)))
		panic("new_inode with unknown device");

	j = 8192;
	for (i=0 ; i<8 ; i++) {
		if ((bh = sb->s_imap[i])) {
			if ((j=find_first_zero(bh->b_data))<8192)
				break;
		}
	}

	if (!bh || j >= 8192 || j+i*8192 > sb->s_ninodes) {
		panic("not implemented!\n");
		//iput(inode);
		return NULL;
	}

	if (test_and_set_bit(j,bh->b_data))
		panic("new_inode: bit already set");

	bh->b_dirt = 1;
	inode->i_count=1;
	inode->i_nlinks=1;
	inode->i_dev=dev;
//	inode->i_uid=current->euid;
//	inode->i_gid=current->egid;
	inode->i_dirt=1;
	inode->i_num = j + i*8192;
//	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;

	return inode;
}
