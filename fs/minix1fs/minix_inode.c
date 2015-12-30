#include <fuckOS/minixfs.h>
#include <fuckOS/dcache.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <sys/stat.h>

#include <string.h>


struct dentry* minix_lookup(char *,struct dentry *,int);

struct inode_operations minix_inode_op = 
{
	
};






