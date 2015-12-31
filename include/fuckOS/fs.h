#ifndef _MINIOS_FS_H
#define _MINIOS_FS_H
#include <types.h>

#include <fuckOS/list.h>
#include <fuckOS/dcache.h>
#include <fuckOS/namei.h>

#include <asm/atomic.h>


#define BLOCK_SIZE 		1024
#define MAX_FILE		128
#define	MAX_DENTRY_HASH		1024

struct file_system_type {
	const char *name;
	struct super_block *(*get_super)(struct file_system_type *,int ,void *);
	int (*fs_init) (void *);
	struct list_head next;
	struct list_head fs_supers;
};


struct inode;
struct super;
struct inode_operations;
struct super_operations;
struct file_operations;

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
	int i_num;
	int i_state;
	int i_dev;
	int i_mode;
	int i_nlink;
	atomic_t i_count;
	const struct inode_operations *i_op;
	const struct file_operations *i_fop;
	struct super_block *i_sb;
	struct hlist_node i_hash;
	void* i_inode;
};

struct inode_operations {
	struct dentry * (*lookup) (struct inode *,struct dentry *, struct nameidata*);
	int (*create) (struct inode *,struct dentry *, int, struct nameidata*);
	int (*mkdir) (struct inode *,struct dentry *,int);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct inode *,struct dentry *,int,int);
	int (*rename) (struct inode *, struct dentry *,struct inode *, struct dentry *);
};
static struct inode_operations empty_iops = 
{

};
    
struct super_block {
	struct list_head s_list;
	uint32_t s_max_size;
	uint16_t s_magic;
	uint32_t s_blocksize;
	int32_t s_dev;
	const struct super_operations *s_op;
	void* s_super;
	struct dentry *s_root;
};

struct super_operations {
	struct inode *(*alloc_inode)(struct super_block *sb);
	void (*destroy_inode)(struct inode *);
	int (*read_inode)(struct inode *);
	int (*read_super)(struct super_block * , void * , int );
};

struct file {
	uint32_t f_mode;
	uint32_t f_flags;
	uint32_t f_count;
	struct inode *f_inode;
	uint32_t f_pos;
	struct file_operations *f_op;
};

struct file_operations {
	int (*read)(struct file * ,char*, int ,int);
};

struct files_struct {
	struct file* fd[MAX_FILE];
};

struct fs_struct {
	struct dentry *root;
	struct vfsmount *mnt_root;
	struct dentry *pwd;
	struct vfsmount *mnt_pwd;
};

struct vfsmount {
	struct vfsmount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct dentry *mnt_root;
	struct super_block *mnt_sb;
	struct list_head mnt_mounts;
	struct list_head mnt_child;
	struct hlist_node mnt_hash;
	const char *mnt_devname;
	spinlock_t mnt_lock;
	int mnt_count;
};

extern void ide_init();

//fs/vfsmount.c
extern struct vfsmount *alloc_vfsmnt(const char*);
extern void insert_vfsmnt(struct vfsmount *,struct dentry *);
extern struct vfsmount *lookup_vfsmnt(struct vfsmount *,struct dentry*);

//fs/buffer.c
extern struct buffer_head *buffer_read(int,uint32_t);

//fs/file_system.c
extern int register_filesystem(struct file_system_type *);
extern void filesystem_init();
extern struct super_block *find_super_block(struct file_system_type *,int);
//fs/inode.c
extern struct inode * new_inode(struct super_block *);
extern int  path_lookup(char *, int,uint32_t , struct nameidata *) ;

//fs/super.c
extern struct super_block* get_sb_nodev(struct file_system_type *,int ,void *);


//fs/file.c
extern int get_empty_file(struct file **);
extern int find_file(int,struct file **,int);
#endif/*_MINIOS_FS_H*/
