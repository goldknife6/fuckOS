#include <fuckOS/ramfs.h>
#include <fuckOS/assert.h>
#include <fuckOS/libfs.h>
#include <fuckOS/task.h>
#include <sys/stat.h>
#include <string.h>

#include <mm/slab.h>
static int ramfs_init(void*);
static void _create_file(struct inode *inode,uint8_t *start,uint8_t *end);
static void create_file(struct dentry *parent);

struct file_system_type ramfs_fs = 
{
	.name = "ramfs",
	//.get_super = ramfs_get_super,
	.fs_init = ramfs_init,
	.next = LIST_HEAD_INIT(ramfs_fs.next),
	.fs_supers = LIST_HEAD_INIT(ramfs_fs.fs_supers),
};

static int ramfs_init(void* data)
{
	printk("registering %s file system!\n",ramfs_fs.name);
	
	int res;
	struct super_block *sb;
	struct dentry *parent;
	struct qstr name;

	create_file(root_dentry);
	
	return 0;
}

static void _create_file(struct inode *inode,uint8_t *start,uint8_t *end)
{
	struct ram_file *ram;
	ram = kmalloc(sizeof(struct ram_file));
	assert(ram);
	
	inode->i_inode = ram;
	ram->start = start;
	ram->size = end - ram->start;
}

#define CREATE(name,x,p)									\
	do {											\
		struct inode *inode;								\
		extern uint8_t TASK_PASTE3(_binary_, x, _start)[];				\
		extern uint8_t TASK_PASTE3(_binary_, x, _end)[];				\
		inode = create_node(p, &(struct qstr) { name, strlen(name)},S_IFREG,		\
				&ramfs_file_operations,&ramfs_dir_inode_operations);		\
		_create_file(inode,TASK_PASTE3(_binary_, x, _start),TASK_PASTE3(_binary_, x, _end));\
	} while (0)

static void create_file(struct dentry *parent)
{
	CREATE("hello",hello,parent);
	CREATE("forktree",forktree,parent);
	CREATE("sh",sh,parent);
	CREATE("echo",echo,parent);
	CREATE("num",num,parent);
	CREATE("cat",cat,parent);
	CREATE("shell_sh",shell_sh,parent);
}
