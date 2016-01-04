#include <fuckOS/ttyfs.h>
#include <fuckOS/assert.h>
#include <sys/stat.h>
#include <mm/slab.h>

static int ttyfs_init(void*);
static struct super_block *ttyfs_get_super(struct file_system_type *,int ,void *);
static struct dentry *ttyfs_find_dev(struct dentry *);

struct file_system_type tty_fs = 
{
	.name = "ttyfs",
	.get_super = ttyfs_get_super,
	.fs_init = ttyfs_init,
	.next = LIST_HEAD_INIT(tty_fs.next),
	.fs_supers = LIST_HEAD_INIT(tty_fs.fs_supers),
};

static struct dentry *ttyfs_find_dev(struct dentry *root)
{
	struct dentry *dentry;
	dentry = dentry_lookup(root,&(struct qstr) { "dev", 3});
	return dentry;
}

static int ttyfs_init(void *data)
{
	printk("registering %s file system!\n",tty_fs.name);
	
	int res;
	struct super_block *sb;
	struct dentry *parent;
	struct qstr name;

	sb = ttyfs_get_super(&tty_fs,0,NULL);

	assert(sb);

	list_add(&sb->s_list,&tty_fs.fs_supers);
	parent = ttyfs_find_dev(root_dentry);
	assert(parent);
	create_node(parent, &(struct qstr) { "tty", 3},S_IFCHR,
				&ttyfs_file_op,&ttyfs_inode_op);
	return 0;
}

static struct super_block *
ttyfs_get_super(struct file_system_type *fs_type,int dev,void *data)
{
	struct super_block *s;
	s = find_super_block(fs_type,dev);
	if (s)
		return s;
	s = new_sb(fs_type,dev,data);
	s->s_op = &ttyfs_super_op;
	s->s_blocksize = PAGE_SIZE;
	s->s_magic = TTYFS_MAGIC;
	return s;
}

