#include <fuckOS/fs.h>
#include <mm/slab.h>
#include <string.h>

#define MAX_VFSMNT_HASH 1024

static struct hlist_head vfsmnt_hashtable[MAX_VFSMNT_HASH];
STATIC_INIT_SPIN_LOCK(vfsmnt_lock);

struct vfsmount *root_vfsmnt;

struct vfsmount *alloc_vfsmnt(const char* name)
{
	struct vfsmount *mnt;
	mnt = kmalloc(sizeof(struct vfsmount));
	assert(mnt);
	memset(mnt,0,sizeof(struct vfsmount));
	mnt->mnt_devname = name;
	INIT_LIST_HEAD(&mnt->mnt_mounts);
	INIT_LIST_HEAD(&mnt->mnt_child);
	INIT_HLIST_NODE(&mnt->mnt_hash);
	SPIN_LOCK_INIT(&mnt->mnt_lock);
	return mnt;
}

void insert_vfsmnt(struct vfsmount *mnt,
			struct dentry *dentry)
{
	uint32_t hash;
	if (dentry && mnt && mnt->mnt_parent) {
		hash = (uint32_t)dentry ^ (uint32_t)mnt->mnt_parent;
		hash <<= 5;
		struct hlist_head *hlist = &vfsmnt_hashtable[hash%MAX_VFSMNT_HASH];
		hlist_add_head(&mnt->mnt_hash, hlist);
	} else {
		panic("");	
	}
}

struct vfsmount *
lookup_vfsmnt(struct vfsmount *mnt, 
			struct dentry* dentry)
{
	uint32_t hash;
	struct vfsmount *tmp;
	hash = (uint32_t)dentry ^ (uint32_t)mnt;
	hash <<= 5;
	struct hlist_head *hlist = &vfsmnt_hashtable[hash%MAX_VFSMNT_HASH];

	hlist_for_each_entry(tmp, hlist, mnt_hash){
		if (tmp->mnt_parent == mnt) {
			return tmp;
		}
	}
	return NULL;
}
