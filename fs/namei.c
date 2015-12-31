#include <fuckOS/fs.h>
#include <fuckOS/task.h>

#include <sys/stat.h>

static int link_path_lookup(char* , struct nameidata *);
static int check_permission(struct inode* ,struct nameidata *);

static int follow_mount(struct path *path)
{
	struct vfsmount *mounted;
	mounted = lookup_vfsmnt(path->mnt, path->dentry);
	if (mounted) {
		path->mnt = mounted;
		path->dentry = mounted->mnt_root;
		return 1;
	}
	return 0;
}


static struct dentry *
real_lookup(struct dentry *dentry,
		struct qstr *qstr,struct nameidata *nd)
{

	return NULL;
}

static int do_lookup(struct nameidata *nd, 
			struct qstr *name,
			struct path *path)
{
	struct dentry* dentry;
	struct vfsmount *mnt = nd->mnt;

	dentry = dentry_lookup(nd->dentry,name);

	if (!dentry)
     		goto need_lookup;

done:
	path->mnt = mnt;
  	path->dentry = dentry;
	follow_mount(path);
	return 0;
fail:
	return -ENOENT;

need_lookup:
	dentry = real_lookup(nd->dentry, name, nd);

	if (!dentry)
      		goto fail;
  	goto done;
}

int path_lookup(char *name,int len,uint32_t flags, 
				struct nameidata *nd) 
{
	struct inode* inode;

	if ( *name == '/' ) {
		nd->mnt = curtask->fs->mnt_root;
		nd->dentry = curtask->fs->root;
	} else if (*name) {
		nd->mnt = curtask->fs->mnt_pwd;
		nd->dentry = curtask->fs->pwd;
	} else {
		return -ENOENT;
	}
	nd->flags = flags;
	return link_path_lookup(name,nd);
}

static int check_permission(struct inode* inode,
				struct nameidata *nd)
{
	if (!S_ISDIR(inode->i_mode)) {
		return -ENOTDIR;
	}
	return 0;
}

static int
link_path_lookup(char* name, struct nameidata *nd)
{
	int res;
	struct inode *inode;
	struct path next;
	int lookup_flags = nd->flags;
	struct qstr this;

	while (*name == '/')
     		name++;

	res = -ENOENT;

	if (!(*name))
      		goto return_reval;

	inode = nd->dentry->d_inode;

	for(;;) {
		char c;
		int namelen = 0;
		struct dentry *dir;

		if ((res = check_permission(inode,nd)) < 0)
			goto return_reval;

		c = *name;

		while (c && c != '/') {
			name++;
			this.name[namelen] = c;
			c = *name;
			 ++namelen;
		}
		this.name[namelen] = 0;
		this.len = namelen;

		if (!c)
        		goto last_component;

		while (*(++name) == '/');
	
    		if (!(*name))
       			goto last_with_slashes;

		/*
		 * "." and ".." are special
		 */
		if (this.name[0] == '.') switch (this.len) {
			default:
	       			break;
			case 2:
				if (this.name[1] != '.')
		   			break;
				nd->dentry = nd->dentry->d_parent;
				inode = nd->dentry->d_inode;
				 /* fallthrough */
			case 1:
	       			continue; 
		}
		
		if ((res = do_lookup(nd, &this, &next)) < 0)
			goto return_reval;

		inode = next.dentry->d_inode;
		if (!inode)
			goto return_reval;

		nd->mnt = next.mnt;
		nd->dentry = next.dentry;
	}

last_with_slashes:
	res =  0;

last_component:
	if (lookup_flags & LOOKUP_PARENT)
        	goto lookup_parent;
	
	if (this.name[0] == '.') switch (this.len) {
			default:
	       			break;
			case 2:
				if (this.name[1] != '.')
		   			break;
				nd->dentry = nd->dentry->d_parent;
				inode = nd->dentry->d_inode;
				 /* fallthrough */
			case 1:
	       			goto return_reval;
	}
	print_qsrt(&this);

	res = do_lookup(nd, &this, &next);
	if (res < 0)
		goto return_reval;

	//follow link!!!!

	//inode = next.dentry->d_inode;

	nd->mnt = next.mnt;
	nd->dentry = next.dentry;

	goto return_reval;

lookup_parent:
	nd->last = this;
	nd->last_type = LAST_NORM;

	if (this.name[0] != '.')
		goto return_reval;
	if (this.len == 1)
		nd->last_type = LAST_DOT;
	if (this.len == 1)
		nd->last_type = LAST_DOTDOT;
return_reval:
	return res;
}
