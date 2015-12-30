#include <fuckOS/fs.h>

#include <mm/slab.h>

#include <string.h>
#include <errno.h>

struct file_system_type * find_filesystem(const char *, unsigned );
struct super_block *find_super_block(struct file_system_type *,int);

struct list_head file_system = LIST_HEAD_INIT(file_system);

STATIC_INIT_SPIN_LOCK(file_systems_lock);

extern struct file_system_type root_fs;
extern struct file_system_type minix_fs;
extern struct file_system_type ramfs_fs;

struct file_system_type *filesystem [] = 
{
	&ramfs_fs,
	&root_fs,
	&minix_fs,
	NULL
};

void filesystem_init()
{
	struct file_system_type **fs = filesystem;
	while (*fs) {
		register_filesystem(*fs);
		fs++;
	}
}

int register_filesystem(struct file_system_type * fs)
{
        int res = 0;

        if (!list_empty(&fs->next))
                return -EBUSY;

        spin_lock(&file_systems_lock);

        if (find_filesystem(fs->name, strlen(fs->name))) {
                res = -EBUSY;   
	} else {
        	INIT_LIST_HEAD(&fs->fs_supers);
		INIT_LIST_HEAD(&fs->next);  
		list_add(&fs->next,&file_system);
		res = fs->fs_init(NULL);
	}                        
        spin_unlock(&file_systems_lock); //解锁
        return res;
}


struct file_system_type *
find_filesystem(const char *name, unsigned len)
{
	struct file_system_type *fs;
	list_for_each_entry(fs, &file_system, next) {
		if (strlen(fs->name) == len &&
                    strncmp(fs->name, name, len) == 0)
                        return fs;
	}
      
        return NULL;
}


struct super_block *
find_super_block(struct file_system_type *fs,int dev)
{
	struct super_block *sb;
	assert(fs);
	list_for_each_entry(sb, &fs->fs_supers, s_list) {
		if (sb->s_dev == dev)
                        return sb;
	}
	return NULL;
}



