#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <sys/stat.h>

#include <mm/slab.h>

#include <string.h>

#define MAX_HASH_INODE	1024
#define inode_hashfn(dev,num) (((uint32_t)((dev)^(num)))%MAX_HASH_INODE)
#define inode_hash(dev,num) inode_table[inode_hashfn(dev,num)]


static struct dentry* find_dentry(struct dentry *,char *,int);
STATIC_INIT_SPIN_LOCK(inode_lock);


struct hlist_head inode_table[MAX_HASH_INODE];

static struct inode *alloc_inode(struct super_block *sb)
{
	struct inode *inode;

	if (sb->s_op->alloc_inode) {
                inode = sb->s_op->alloc_inode(sb);
	} else {
		inode = kmalloc(sizeof(struct inode));
	}

	if (inode) {
		inode->i_sb = sb;
		inode->i_dev = sb->s_dev;
		inode->i_op = &empty_iops;
		inode->i_inode = NULL;
		inode->i_mode = 0;
		atomic_set(&inode->i_count, 1);
		INIT_HLIST_NODE(&inode->i_hash);
	}
	return inode;
}


struct inode *new_inode(struct super_block *sb)
{
	static uint32_t last_ino;
	struct inode * inode;

	inode = alloc_inode(sb);
	if (inode) {
		spin_lock(&inode_lock);
		//list_add(&inode->i_list, &inode_in_use);
		inode->i_num = ++last_ino;
		inode->i_state = 0;
		spin_unlock(&inode_lock);
	}
	return inode;
}


int  path_lookup(const char *name,uint32_t flags, 
				struct nameidata *nd) 
{
	struct inode* inode;
	struct dentry *curdir;
	char c;
	const char * thisname;
	if ( (c = *name == '/') ) {
		name++;
		curdir = curtask->fs->root;
	} else if (c) {
		curdir = curtask->fs->pwd;
	} else {
		return -ENOENT;
	}
	inode = curdir->d_inode;
	while (1) {
		thisname = name;
		if (!S_ISDIR(inode->i_mode)) {
			return -ENOENT;
		}
		
	}
	return 0;
}
