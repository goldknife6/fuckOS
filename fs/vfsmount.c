#include <fuckOS/fs.h>
#include <mm/slab.h>
#include <string.h>

/*
struct vfsmount {
	struct vfsmount *mnt_parent;
	struct dentry *mnt_mountpoint;
	struct dentry *mnt_root;
	struct super_block *mnt_sb;
	struct list_head mnt_mounts;
	struct list_head mnt_child;
	const char *mnt_devname;
	spinlock_t mnt_lock;
	int mnt_count;
};

*/
struct vfsmount *alloc_vfsmnt(const char* name)
{
	struct vfsmount *mnt;
	mnt = kmalloc(sizeof(struct vfsmount));
	assert(mnt);
	memset(mnt,0,sizeof(struct vfsmount));
	mnt->mnt_devname = name;
	INIT_LIST_HEAD(&mnt->mnt_mounts);
	INIT_LIST_HEAD(&mnt->mnt_child);
	SPIN_LOCK_INIT(&mnt->mnt_lock);
	return mnt;
}
