#ifndef _MINIOS_FS_H
#define _MINIOS_FS_H
#include <types.h>
#include <fuckOS/list.h>
#include <asm/atomic.h>

#define BLOCK_SIZE 1024
#define NR_SUPER 8


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
	int s_dev;
	struct list_head    s_list;               /* 指向所有超级块的链表 */
	//struct super_operations    *s_op; /* 超级块方法 */
	struct dentry        *s_root;           /* 目录挂载点 */
	int    s_count;                   /* 超级块引用计数 */
	struct list_head    s_inodes;           /* inode链表 */
};
struct m_inode {
};

extern void ide_init();
extern void buffer_init();
extern struct buffer_head *buffer_read(int, uint32_t);
#endif/*_MINIOS_FS_H*/
