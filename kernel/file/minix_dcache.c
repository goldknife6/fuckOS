#include <fuckOS/dcache.h>
#include <fuckOS/list.h>

static int minix_dentry_hash(const struct dentry *, const char *,int);



struct dentry_operations minix_dentry_op = 
{
	.d_hash = minix_dentry_hash
};




static int 
minix_dentry_hash(const struct dentry *parent,const char* str,int len)
{
	int hash = _hash(str,len);
	hash += ((uint32_t) parent ^ GOLDEN_RATIO_PRIME);
        hash = hash ^ (hash ^ GOLDEN_RATIO_PRIME);
	return hash;
}


