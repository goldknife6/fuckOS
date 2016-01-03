#include <fuckOS/fs.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>
#include <sys/stat.h>
#include <string.h>

static int pipefs_read_super(struct super_block *,void*,int);

struct super_operations pipefs_super_op = 
{
	.read_super = pipefs_read_super,
};


static int pipefs_read_super(struct super_block *sb,
				void* data,int slient)
{
	
	return 0;
}
