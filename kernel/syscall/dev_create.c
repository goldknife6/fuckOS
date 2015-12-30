#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

#include <mm/slab.h>

#include <errno.h>
#include <string.h>

/*

struct nameidata {
	struct dentry *dentry;
	struct vfsmount *mnt;
	struct qstr last;
	uint32_t flags;
	int last_type;
};

*/
static char* get_last(char *,int);
int create(char *pathname,int len,int mode)
{
	struct nameidata nd;
	struct dentry *dentry,*parent;
	struct inode *inode;
	struct qstr qstr;
	int res;
	res = path_lookup(pathname,len,0,&nd);
	if (res < 0)
		return res;

	parent = nd.dentry;
	inode = parent->d_inode;

	if (!strncmp(nd.last.name,get_last(pathname,len),nd.last.len)) {
		dentry = dentry_lookup(parent,nd.last.name,len);
		if (dentry)
			return -EEXIST;
		dentry = d_alloc(parent, &nd.last);
		if (!dentry)
			return -ENOMEM;
		res = inode->i_op->create(inode,dentry,mode,&nd);
		if (res < 0)
			return res;
		dentry_insert(parent,dentry);
	}
	return res;
}


static char* get_last(char *pathname,int len)
{
	char *p = pathname + len;
	
	while (*p != '/' && p >= pathname) {
		p--;
	}
	return ++p;
}
