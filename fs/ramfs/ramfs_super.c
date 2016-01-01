#include <fuckOS/ramfs.h>
#include <sys/stat.h>

#include <mm/pages.h>
#include <errno.h>
static int ramfs_read_super(struct super_block * , void * , int );

static struct super_operations ramfs_super_ops = 
{
	.read_super = ramfs_read_super
};

static int
ramfs_read_super(struct super_block * sb, 
			void * data, int silent)
{
	
	return 0;
}

