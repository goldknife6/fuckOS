#ifndef _MINIOS_FS_H
#define _MINIOS_FS_H
#include <types.h>
#include <fuckOS/list.h>
#include <asm/atomic.h>

#define BLOCK_SIZE 	1024

#define NAME_LEN 	30

#define SUPER_MAGIC 	0x138F

#define I_MAP_SLOTS	8
#define Z_MAP_SLOTS 	8

struct buffer_head {
	int8_t b_data[BLOCK_SIZE];		/* pointer to data block (1024 bytes) */
	uint32_t b_blocknr;			/* block number */
	uint32_t b_dev;				/* device (0 = free) */
	uint8_t b_uptodate;			/* 1-valid,0-invalid */
	uint8_t b_dirt;				/* 0-clean,1-dirty */
	uint8_t b_count;			/* users using this block */
	spinlock_t b_lock;			/* 0 - ok, 1 -locked */
	struct hlist_node  list_free;
};


struct super_block {
	uint16_t s_ninodes;
	uint16_t s_nzones;
	uint16_t s_imap_blocks;
	uint16_t s_zmap_blocks;
	uint16_t s_firstdatazone;
	uint16_t s_log_zone_size;
	uint32_t s_max_size;
	uint16_t s_magic;
/* These are only in memory */
	struct buffer_head * s_imap[I_MAP_SLOTS];
	struct buffer_head * s_zmap[Z_MAP_SLOTS];
	uint16_t s_dev;
	struct m_inode * s_isup;
	struct m_inode * s_imount;
	uint32_t s_time;
	//struct task_struct * s_wait;
	uint8_t s_lock;
	uint8_t s_rd_only;
	uint8_t s_dirt;

	struct list_head    s_list;               /* 指向所有超级块的链表 */
	struct list_head    s_inodes;           /* inode链表 */
	struct inode * s_inode;
};

struct d_inode {
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size;
	uint32_t i_time;
	uint8_t i_gid;
	uint8_t i_nlinks;
	uint16_t i_zone[9];
};


struct m_inode {
	uint16_t i_mode;
	uint16_t i_uid;
	uint32_t i_size;
	uint32_t i_mtime;
	uint8_t i_gid;
	uint8_t i_nlinks;
	uint16_t i_zone[9];
/* these are in memory also */
	uint32_t i_atime;
	uint32_t i_ctime;
	uint16_t i_dev;
	uint16_t i_num;
	uint16_t i_count;
	uint8_t i_lock;
	uint8_t i_dirt;
	uint8_t i_pipe;
	uint8_t i_mount;
	uint8_t i_seek;
	uint8_t i_update;
};


struct d_super_block {
	uint16_t s_ninodes;
	uint16_t s_nzones;
	uint16_t s_imap_blocks;
	uint16_t s_zmap_blocks;
	uint16_t s_firstdatazone;
	uint16_t s_log_zone_size;
	uint32_t s_max_size;
	uint16_t s_magic;
};

struct dir_entry {
	uint16_t inode;
	int8_t name[NAME_LEN];
};

extern void mount_root();
extern void ide_init();
extern void buffer_init();
extern struct buffer_head *buffer_read(int, uint32_t);
#endif/*_MINIOS_FS_H*/
