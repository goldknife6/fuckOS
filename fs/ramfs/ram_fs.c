#include <fuckOS/ramfs.h>
#include <fuckOS/assert.h>


static int ramfs_init(void*);

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
	return 0;
}
