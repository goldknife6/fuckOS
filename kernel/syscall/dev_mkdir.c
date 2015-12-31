#include <fuckOS/fs.h>
#include <fuckOS/assert.h>

#include <sys/stat.h>


#include <errno.h>
#include <string.h>

int mkdir(char *pathname,int len,int mode)
{
	struct nameidata nd;
	struct dentry *dentry,*parent;
	struct inode *inode;

	int res;
	res = path_lookup(pathname,len,LOOKUP_PARENT|LOOKUP_FOLLOW,&nd);
	if (res < 0)
		return res;

	parent = nd.dentry;
	inode = parent->d_inode;
	if (!strncmp(nd.last.name,get_last(pathname,len),nd.last.len)) {
		dentry = dentry_lookup(parent,&nd.last);
		if (dentry)
			return -EEXIST;
		dentry = d_alloc(parent, &nd.last);
		if (!dentry)
			return -ENOMEM;
		res = inode->i_op->mkdir(inode,dentry,mode);
		if (res < 0)
			return res;
		dentry_insert(parent,dentry);
	}
	return res;
}

