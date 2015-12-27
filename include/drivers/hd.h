#ifndef _MINIOS_HD_H
#define _MINIOS_HD_H
#include <types.h>


#define MAX_DRIVES		2
#define NR_PART_DRIVE		4
#define NR_PRIM_DRIVE		(NR_PART_DRIVE + 1)
#define NR_LOG_PART		16
#define NR_LOG_DRIVE		(NR_LOG_PART*NR_PART_DRIVE)
#define MAX_PRIM		(MAX_DRIVES * NR_PRIM_DRIVE - 1)

#define MAJOR_SHIFT		8

#define MAJOR(x)		(((x) >> MAJOR_SHIFT) & 0xFF)
#define MINOR(x)		((x) & 0xFF)
#define DRV_OF_DEV(dev)		((dev) <= MAX_PRIM ? (dev) / NR_PRIM_DRIVE : ((dev) - MINOR_hd1a)/NR_LOG_DRIVE)
#define	MAKE_DEV(major,minor)	((major) << MAJOR_SHIFT | (minor))
#define	MINOR_hd1a		0x10

#define	EXT_PART		0x5
//AT 硬盘接口寄存器
#define HD_DATA		0x1f0
#define HD_ERROR	0x1f1
#define HD_NSECTOR	0x1f2
#define HD_SECTOR	0x1f3
#define HD_LCYL		0x1f4
#define HD_HCYL		0x1f5
#define	HD_CURRENT	0x1f6
#define	HD_STATUS	0x1f7
#define	HD_CMD		0x3f6

//主状态寄存器 mask
	
#define ERR_STAT	0x01
#define	INDEX_STAT	0x02
#define	ECC_STAT	0x04
#define	DRQ_STAT	0x08
#define	SEEK_STAT	0x10
#define	WRERR_STAT	0x20
#define	READY_STAT	0x40
#define	BUSY_STAT	0x80


#define WRITE		0x30
#define READ		0x20


struct hd_struct {
	int32_t head;
	int32_t sect;
	int32_t cyl;
	int32_t wpcom;
	int32_t lzone;
	int32_t ctl;
};

struct partable {
	int8_t boot_ind;	/* 0x80 - active (unused) */
	int8_t head;		/* ? */
	int8_t sector;		/* ? */
	int8_t cyl;		/* ? */
	int8_t sys_ind;		/* ? */
	int8_t end_head;	/* ? */
	int8_t end_sector;	/* ? */
	int8_t end_cyl;		/* ? */
	int32_t start_sect;	/* starting sector counting from 0 */
	int32_t nr_sects;	/* nr of sectors in partition */
};

struct part_info {
	long start_sect;
	long nr_sects;
	int8_t sys_ind;
};

struct hd_info
{
	int			open_cnt;
	struct part_info	primary[NR_PRIM_DRIVE];
	struct part_info	logical[NR_LOG_DRIVE];
};



extern void ide_init(void);
#endif/*_MINIOS_HD_H*/
