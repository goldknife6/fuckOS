#ifndef _MINIOS_FS_H
#define _MINIOS_FS_H
#include <types.h>

#include <asm/atomic.h>

struct buffer_head {
	int8_t * b_data;			/* pointer to data block (1024 bytes) */
	uint32_t b_blocknr;			/* block number */
	int32_t b_dev;				/* device (0 = free) */
	uint8_t b_dirt;				/* 0-clean,1-dirty */
	uint8_t b_count;			/* users using this block */
	spinlock_t b_lock;			/* 0 - ok, 1 -locked */
	struct list_head  list_free;
};

struct super_block {
	struct list_head    s_list;               /* 指向所有超级块的链表 */
	//struct super_operations    *s_op; /* 超级块方法 */
	struct dentry        *s_root;           /* 目录挂载点 */
	int    s_count;                   /* 超级块引用计数 */
	struct list_head    s_inodes;           /* inode链表 */
};

#endif/*_MINIOS_FS_H*/
