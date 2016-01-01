#include <fuckOS/ttyfs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int ttyfs_read_super(struct super_block *,void*,int);

struct super_operations ttyfs_super_op = 
{
	.read_super = ttyfs_read_super,
};


static int ttyfs_read_super(struct super_block *sb,
				void* data,int slient)
{
	
	return 0;
}
