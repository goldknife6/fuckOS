/*
 * Minimal PIO-based (non-interrupt-driven) IDE driver code.
 * For information about what all this IDE/ATA magic means,
 * see the materials available on the class references page.
 */

#include <asm/x86.h>


#include <fuckOS/assert.h>
#include <fuckOS/hd.h>
#include <fuckOS/fs.h>

#include <mm/layout.h>

#define IDE_BSY		0x80
#define IDE_DRDY	0x40
#define IDE_DF		0x20
#define IDE_ERR		0x01
#define SECTSIZE	512


#define P_PRIMARY	1
#define P_EXTENDED	2


static int diskno = 1;

struct hd_struct hd_struct[MAX_DRIVES];
struct hd_info hd_info[MAX_DRIVES];


static int NR_HD = 0;
static void sys_setup(void);
static void phdinfo();
static void print_partition();
static int get_part_table(int ,int ,struct partable*);
static void partition(int device, int style);

int ide_read(uint32_t secno, void *dst, uint32_t driver ,size_t nsecs);

void 
ide_init()
{
	sys_setup();
}

static void 
sys_setup(void)
{
	struct buffer_head *buf;
	struct partable *p = NULL;
	struct partable entry[NR_PART_DRIVE];
	int *bios = (int*)(KERNEL_BASE_ADDR + 0x104);
	void *BIOS = (void *)(((((*bios) >> 16) & 0xFFFF) << 4) + ((*bios) & 0xFFFF));
	BIOS = BIOS + KERNEL_BASE_ADDR;
	int i,drive;
	
	for(drive = 0;drive < 2;drive++) {
		hd_struct[drive].cyl = *(uint8_t *) BIOS;
		hd_struct[drive].head = *(uint8_t *) (2+BIOS);
		hd_struct[drive].wpcom = *(uint8_t *) (5+BIOS);
		hd_struct[drive].ctl = *(uint8_t *) (8+BIOS);
		hd_struct[drive].lzone = *(uint8_t *) (12+BIOS);
		hd_struct[drive].sect = *(uint8_t *) (14+BIOS);
		BIOS += 16;
	}
	

	if (hd_struct[1].cyl)
		NR_HD=2;
	else
		NR_HD=1;
	phdinfo();

	for (i=0 ; i<MAX_DRIVES ; i++) {
		hd_info[i].primary->start_sect = 0;
		hd_info[i].primary->nr_sects = hd_struct[i].head*
				hd_struct[i].sect*hd_struct[i].cyl;
	}
	printk("getting partition table\n");
	for (drive=0 ; drive<NR_HD ; drive++) {
		partition(drive * (NR_PART_DRIVE+1) , P_PRIMARY);
	}
	print_partition();
}

static void print_partition()
{
	int i ;
	for (i = 0; i < NR_HD; i++) {
		int j;
		for (j = 0; j < NR_PRIM_DRIVE; j++) {
			if (hd_info[i].primary[j].nr_sects)
			printk("primary[%d] start_sect:%d nr_sects:%d\n",j,
			hd_info[i].primary[j].start_sect,hd_info[i].primary[j].nr_sects);

			if (hd_info[i].primary[j].sys_ind == EXT_PART) {
				int k;
				for (k = 0; k < NR_LOG_DRIVE; k++) {
					if (hd_info[i].logical[k].nr_sects)
					printk("       logical[%d] start_sect:%d nr_sects:%d\n",
					k,hd_info[i].logical[k].start_sect,hd_info[i].logical[k].nr_sects);
				}
			}
		}
	}
}
static void partition(int device, int style)
{
	int i;
	int drive = DRV_OF_DEV(device);
	struct hd_info * hdi = &hd_info[drive];
	struct partable part_tbl[NR_LOG_DRIVE];

	if (style == P_PRIMARY) {
		get_part_table(drive, 0, part_tbl);

		int nr_prim_parts = 0;
		for (i = 0; i < NR_PART_DRIVE; i++) { /* 0~3 */
			if (part_tbl[i].sys_ind == 0) 
				continue;

			nr_prim_parts++;
			int dev_nr = i + 1;		  /* 1~4 */
			hdi->primary[dev_nr].start_sect = part_tbl[i].start_sect;
			hdi->primary[dev_nr].nr_sects = part_tbl[i].nr_sects;
			if (part_tbl[i].sys_ind == EXT_PART) /* extended */ {
				hdi->primary[dev_nr].sys_ind = EXT_PART;
				partition(device + dev_nr, P_EXTENDED);
			}
		}
		assert(nr_prim_parts != 0);
	}
	else if (style == P_EXTENDED) {
		int j = device % NR_PRIM_DRIVE; /* 1~4 */
		int ext_start_sect = hdi->primary[j].start_sect;
		int s = ext_start_sect;
		int nr_1st_sub = (j - 1) * NR_LOG_PART; /* 0/16/32/48 */

		for (i = 0; i < NR_LOG_PART; i++) {
			int dev_nr = nr_1st_sub + i;/* 0~15/16~31/32~47/48~63 */

			get_part_table(drive, s, part_tbl);

			hdi->logical[dev_nr].start_sect = s + part_tbl[0].start_sect;
			hdi->logical[dev_nr].nr_sects = part_tbl[0].nr_sects;
			s = ext_start_sect + part_tbl[1].start_sect;
			/* no more logical partitions
			   in this extended partition */
			if (part_tbl[1].sys_ind == 0)
				break;
		}
	}
	else {
		assert(0);
	}
}

static int get_part_table(int drive,int secno,struct partable* entry)
{
	char dst[512];
	int j,retval;
	struct partable *p;
	retval = ide_read(secno, dst , drive ,1);
	if (retval < 0) {
		panic("");
		return retval;	
	}
/*
	if (secno == 0) {
		if (dst[510] != 0x55 || (unsigned char)
		    dst[511] != 0xAA) {
			panic("Bad partition table on drive %d\n",drive);
		}
	}
*/
	p = (struct partable *)(0x1BE + dst);	

	for (j = 0; j < NR_PART_DRIVE; j++,p++) {
		entry[j] = *p;
	}

	return 0;
}

static void reset_controller(void)
{
	int i;
	outb(HD_CMD,4);
	for(i = 0; i < 100; i++) ;
	outb(HD_CMD,hd_struct[0].ctl & 0x0f);
	
	if ((i = inb(HD_ERROR)) != 1)
		printk("HD-controller reset failed: %02x\n\r",i);
}

static int
ide_wait_ready(bool check_error)
{
	int num = 0x10000;
	int r;

	while (((r = inb(0x1F7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY) {
		if (--num == 0) {
		
		if(r & ERR_STAT)
			printk("ERR_STAT "); 
		if(r & INDEX_STAT)
			printk("INDEX_STAT ");
		if(r & ECC_STAT)
			printk("ECC_STAT ");
		if(r & DRQ_STAT)
			printk("DRQ_STAT ");
		if(r & SEEK_STAT)
			printk("SEEK_STAT ");
		if(r & WRERR_STAT)
			printk("WRERR_STAT ");
		if(r & READY_STAT)
			printk("READY_STAT ");
		if(r & BUSY_STAT)
			printk("BUSY_STAT ");
		}
	}
	if (check_error && (r & (IDE_DF|IDE_ERR)) != 0)
		return -1;
	return 0;
}

static bool
ide_probe_disk1(void)
{
	int r, x;

	// wait for Device 0 to be ready
	ide_wait_ready(0);

	// switch to Device 1
	outb(0x1F6, 0xE0 | (1<<4));

	// check for Device 1 to be ready for a while
	for (x = 0;
	     x < 1000 && ((r = inb(0x1F7)) & (IDE_BSY|IDE_DF|IDE_ERR)) != 0;
	     x++)
		/* do nothing */;

	// switch back to Device 0
	outb(0x1F6, 0xE0 | (0<<4));

	printk("Device 1 presence: %d\n", (x < 1000));
	return (x < 1000);
}

void
ide_set_disk(int d)
{
	if (d != 0 && d != 1)
		panic("bad disk number");
	diskno = d;
}


int
ide_read(uint32_t secno, void *dst, uint32_t driver ,size_t nsecs)
{
	int r;

	assert(nsecs <= 256);
	ide_wait_ready(0);

	outb(0x1F6, 0xE0 | (driver << 4) | ((secno >> 24) & 0x0F));
	outb(0x1F1, 0x00);
	outb(0x1F2, nsecs);
	outb(0x1F3, secno & 0xFF);
	outb(0x1F4, (secno >> 8) & 0xFF);
	outb(0x1F5, (secno >> 16) & 0xFF);
	outb(0x1F6, 0xE0 | ((driver&1)<<4) | ((secno>>24)&0x0F));
	outb(0x1F7, 0x20);	// CMD 0x20 means read sector
	
	for (; nsecs > 0; nsecs--, dst += SECTSIZE) {
		if ((r = ide_wait_ready(1)) < 0)
			return r;
		insl(0x1F0, dst, SECTSIZE/4);
	}
	return 0;
}

int
ide_write(uint32_t secno, const void *src, size_t nsecs)
{
	int r;

	assert(nsecs <= 256);

	ide_wait_ready(0);

	outb(0x1F2, nsecs);
	outb(0x1F3, secno & 0xFF);
	outb(0x1F4, (secno >> 8) & 0xFF);
	outb(0x1F5, (secno >> 16) & 0xFF);
	outb(0x1F6, 0xE0 | ((diskno&1)<<4) | ((secno>>24)&0x0F));
	outb(0x1F7, 0x30);	// CMD 0x30 means write sector

	for (; nsecs > 0; nsecs--, src += SECTSIZE) {
		if ((r = ide_wait_ready(1)) < 0)
			return r;
		outsl(0x1F0, src, SECTSIZE/4);
	}

	return 0;
}

static void 
phdinfo()
{
	int drive;
	for(drive = 0;drive < 2;drive++) {
		printk("cyl:%d head:%d ",hd_struct[drive].cyl,hd_struct[drive].head);
		printk("wpcom:%d ctl:%x ",hd_struct[drive].wpcom,hd_struct[drive].ctl);
		printk("lzone:%d sect:%x\n",hd_struct[drive].lzone,hd_struct[drive].sect);
	}
}
