#include <fuckOS/pipe.h>
#include <fuckOS/assert.h>
#include <sys/stat.h>
#include <mm/slab.h>

static int pipefs_init(void*);
static struct super_block *pipefs_get_super(struct file_system_type *,int ,void *);

struct vfsmount *pipe_mnt;

struct file_system_type pipe_fs = 
{
	.name = "pipefs",
	.get_super = pipefs_get_super,
	.fs_init = pipefs_init,
	.next = LIST_HEAD_INIT(pipe_fs.next),
	.fs_supers = LIST_HEAD_INIT(pipe_fs.fs_supers),
};


static int pipefs_init(void *data)
{
	struct super_block *sb;
	struct vfsmount *mnt;
	int res;

	printk("registering %s file system!\n",pipe_fs.name);
	
	mnt = alloc_vfsmnt(pipe_fs.name);
	assert(mnt);
	pipe_mnt = mnt;
	sb = pipe_fs.get_super(&pipe_fs,0,NULL);

	list_add(&sb->s_list,&pipe_fs.fs_supers);
	res = sb->s_op->read_super(sb,mnt,0);
	return 0;
}

static struct super_block *
pipefs_get_super(struct file_system_type *fs_type,int dev,void *data)
{
	struct super_block *s;
	s = find_super_block(fs_type,dev);
	if (s)
		return s;
	s = new_sb(fs_type,dev,data);
	s->s_op = &pipefs_super_op;
	s->s_blocksize = PAGE_SIZE;
	s->s_magic = PIPEFS_MAGIC;
	return s;
}

