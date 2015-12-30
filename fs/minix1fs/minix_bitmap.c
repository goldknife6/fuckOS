#include <fuckOS/minixfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <asm/bitops.h>

int new_block(struct super_block *sb)
{
	int i,j,dev;
	struct buffer_head *buf;
	struct buffer_head *bmap;

	struct minix_superblock *msb;
	assert(sb);

	dev = sb->s_dev;
	assert(MAJOR(dev == 3));

	assert(sb->s_super);
	msb = (struct minix_superblock*)sb->s_super;

	for (i = 0 ; i < msb->s_zmap_blocks ; i++) {
		bmap = buffer_get(dev,3 + i);
		if (bmap) {
			if ((j = find_first_zero(bmap->b_data)) < 8192)
				break;
		}  else {
			panic("!bmap\n");
		}
	}

	if (i >= 8 || !buf || j >= 8192)
		return 0;

	if (test_and_set_bit(j,bmap->b_data))
		panic("new_block: bit already set");
	
	return 0;
}

struct inode *new_inode(struct minix_superblock* msb)
{
	assert(msb);
	struct inode *inode = NULL;

	return inode;
}

void free_inode(struct minix_superblock* msb)
{
	assert(msb);

}

void free_block(int dev, int block)
{
	
}
