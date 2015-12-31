#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>


int create(char *pathname,int len,int mode)
{
	struct nameidata nd;
	struct dentry *dentry = NULL,*parent;
	struct inode *inode;

	int res;
	res = path_lookup(pathname,len,LOOKUP_PARENT,&nd);
	if (res < 0) {
		return res;
	}
	
	parent = nd.dentry;
	inode = parent->d_inode;

	dentry = dentry_lookup(parent,&nd.last);
	if (dentry) {
		return -EEXIST;
	}
	dentry = d_alloc(parent, &nd.last);
	if (!dentry)
		return -ENOMEM;
	res = inode->i_op->create(inode,dentry,mode,&nd);
	
	if (res < 0)
		return res;

	dentry_insert(parent,dentry);

	return res;
}

