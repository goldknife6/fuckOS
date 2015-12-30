#include <fuckOS/minixfs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>


struct dentry* minix_lookup(char *,struct dentry *,int);

struct inode_operations minix_inode_op = 
{
	.lookup = minix_lookup,
};

static int _bmap(struct inode *,int ,int);
static int bmap(struct inode *,int );

void print_minix_inode(struct inode* inode)
{
	assert(inode);
	assert(inode->i_inode);

	struct minix1_inode *i = (struct minix1_inode *)inode->i_inode;

	printk("size:%d zone[0]:%d time:%d\n",i->i_size,i->i_zone[0],i->i_time);
}
struct inode* get_minix_inode(int num,struct super_block *sb)
{
	struct inode *inode;
	struct minix1_inode* i;

	inode = find_inode(num,sb->s_dev);
	if (inode)
		return inode;
	
	i = kmalloc(sizeof(struct minix1_inode));
	if (!i)
		return NULL;
	memset(i,0,sizeof(struct minix1_inode));
	
	inode = alloc_inode(&minix_inode_op,sb,num,sb->s_dev,(void *)i,0);

	if (!inode) {
		kfree(i);
		return NULL;
	}

	read_inode(sb,inode);

	insert_inode(inode);
	return inode;
}


struct dentry* minix_lookup(char *name,struct dentry *parent,int namelen)
{
	int entries;
	int block,i;
	struct dentry *dentry = NULL;
	struct inode* inode;
	struct minix1_inode *dir;
	struct buffer_head *buf;
	struct minix1_dir_entry *de;

	if (namelen > MINIX1_NAME_LEN)
		return NULL;

	if (namelen <= 0)
		return NULL;

	inode = parent->d_inode;

	dir = (struct minix1_inode *)inode->i_inode;

	entries = dir->i_size / (sizeof (struct minix1_dir_entry));

	if ((namelen == 2) && (name[0] == '.') && (name[1] == '.')) {
		return parent->d_parent;
	} else if ((namelen == 1) && (name[0] == '.')) {
		return parent;
	}


	if (!(block = dir->i_zone[0]))
		return NULL;
	if (!(buf = buffer_read(inode->i_dev,block)))
		return NULL;
	i = 0;
	de = (struct minix1_dir_entry *) buf->b_data;

	while (i < entries) {
		if ((uint8_t *)de >= BLOCK_SIZE + buf->b_data) {
			//brelse(bh);
			buf = NULL;
			if (!(block = bmap(inode,i/DIR_ENTRIES_PER_BLOCK)) ||
			    !(buf = buffer_read(inode->i_dev,block))) {
				i += DIR_ENTRIES_PER_BLOCK;
				continue;
			}
			de = (struct minix1_dir_entry *)buf->b_data;
		}
		if (strncmp(name,de->name,namelen)) {
			inode = get_minix_inode(de->inode,parent->d_sb);
			if (!inode)
				return NULL;
			dentry = alloc_dentry(parent,name,&minix_dentry_op,
						inode,parent->d_sb,namelen);
			return dentry;
		}
		de++;
		i++;
	}

	return NULL;
}


static int _bmap(struct inode *inode,int block,int create)
{
	struct minix1_inode* mi = (struct minix1_inode*)inode->i_inode;
	struct buffer_head * buf;
	int i;

	if (block<0)
		panic("_bmap: block<0");
	if (block >= 7)
		panic("_bmap: block>big");
	if (block < 7) {
		if (create && !mi->i_zone[block]) { 
			panic("not implemented\n");
			/*
			if (inode->i_zone[block]=new_block(inode->i_dev)) {
				//inode->i_ctime=CURRENT_TIME;
				inode->i_dirt=1;
			}
			*/
		}
		return mi->i_zone[block];
	}

	
	return -1;
}


static int bmap(struct inode *inode,int block)
{
	return _bmap(inode,block,0);
}





