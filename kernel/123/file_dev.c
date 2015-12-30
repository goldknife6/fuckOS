#include <fuckOS/fs.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


int file_read(struct m_inode * inode, struct file * filp, char *buf, int count)
{
	int left,chars,nr;
	struct buffer_head * bh;

	if ((left = count) <= 0)
		return 0;

	while (left) {
		nr = bmap(inode,(filp->f_pos)/BLOCK_SIZE);

		if (nr) {
			if ( !(bh = buffer_read(inode->i_dev,nr)) )
				break;
		} else
			bh = NULL;

		nr = filp->f_pos % BLOCK_SIZE;
		chars = MIN( BLOCK_SIZE-nr , left );
		filp->f_pos += chars;
		left -= chars;

		if (bh) {
			uint8_t * p = nr + bh->b_data;
			while (chars-- > 0) {
				*(buf++) = *(p++);
			}
			//brelse(bh);
		} else {
			while (chars-->0) {
				*(buf++) = 0;
			}
		}
	}

	//inode->i_atime = CURRENT_TIME;
	return (count-left)?(count-left):-1;
}
