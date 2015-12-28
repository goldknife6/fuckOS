#ifndef _MINIOS_FS_H
#define _MINIOS_FS_H
#include <types.h>
#include <fuckOS/list.h>
#include <asm/atomic.h>


#define BLOCK_SIZE 		1024
#define MAX_FILE		128
#define	MAX_DENTRY_HASH		1024
struct inode;
struct super;
struct inode_operations;
struct super_operations;

struct buffer_head {
	uint8_t b_data[BLOCK_SIZE];		/* pointer to data block (1024 bytes) */
	uint32_t b_blocknr;			/* block number */
	uint32_t b_dev;				/* device (0 = free) */
	uint8_t b_uptodate;			/* 1-valid,0-invalid */
	uint8_t b_dirt;				/* 0-clean,1-dirty */
	uint8_t b_count;			/* users using this block */
	spinlock_t b_lock;			/* 0 - ok, 1 -locked */
	struct hlist_node  list_free;
};

struct inode {
	const struct inode_operations *i_op;
	struct super_block *i_sb;
	int i_num;
	int i_dev;
	struct hlist_node i_hash;
	void* i_inode;
};
struct inode_operations {

	
};

    
struct super_block {
	struct list_head s_list;
	uint32_t s_max_size;
	uint16_t s_magic;
	uint32_t s_blocksize;
	int32_t s_dev;
	const struct super_operations *s_op;
	void* s_super;
};

struct super_operations {
	struct inode *(*alloc_inode)(struct super_block *sb);
	void (*destroy_inode)(struct inode *);
	int (*read_inode)(struct inode *);
	int (*read_super)(struct super_block *);
};

struct file {
	uint32_t f_mode;
	uint32_t f_flags;
	uint32_t f_count;
	struct inode *f_inode;
	uint32_t f_pos;
};

struct file_struct{
	struct file* fd[MAX_FILE];
	struct inode *root;
	struct inode *pwd;
};

extern void ide_init();
extern struct super_block *get_super(int );
extern struct super_block* alloc_super(int ,uint16_t ,const struct super_operations *,void *,int ,int);
extern int read_inode(struct super_block *,struct inode *);
extern struct buffer_head *buffer_get(int ,uint32_t );
extern struct buffer_head *buffer_read(int ,uint32_t );
extern struct inode *get_inode(struct inode_operations *,struct super_block *,int ,int ,void *);
extern struct inode *find_inode(int ,int );
extern void insert_inode(struct inode *);
#endif/*_MINIOS_FS_H*/
