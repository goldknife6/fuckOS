#include <fuckOS/fs.h>
#include <fuckOS/hd.h>
#include <fuckOS/assert.h>

extern int ide_read(uint32_t , void *, uint32_t ,size_t );
extern struct hd_info hd_info[MAX_DRIVES];

#define NR_BLK_DEV	7


int block_read(struct buffer_head *buf)
{
	assert(buf);

	int retval;
	uint32_t blocknr;
	uint32_t startn = 0;
	uint32_t major = MAJOR(buf->b_dev);
	uint32_t minor = MINOR(buf->b_dev);
	uint32_t drive = DRV_OF_DEV(minor);
	uint32_t logidx = (minor - MINOR_hd1a) % NR_LOG_DRIVE;
	if ( major >= NR_BLK_DEV ) {
		panic("Trying to read nonexistent block-device\n\r");
	}
	
	blocknr = buf->b_blocknr;
	startn = minor < MAX_PRIM ? hd_info[drive].primary[minor%NR_PRIM_DRIVE].start_sect :
		hd_info[drive].logical[logidx].start_sect;
	retval = ide_read(startn + blocknr*2 ,buf->b_data,drive, 2);
	if (!retval)
		buf->b_uptodate = 1;

	return retval;
}
