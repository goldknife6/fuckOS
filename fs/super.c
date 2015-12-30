#include <fuckOS/fs.h>
#include <fuckOS/minixfs.h>
#include <drivers/hd.h>
#include <fuckOS/assert.h>

#include <mm/slab.h>

#include <string.h>

static struct super_block *alloc_sb();


struct super_block* 
get_sb_nodev(struct file_system_type *fs,
				int dev,void *data)
{
	struct super_block* sb;
	sb = find_super_block(fs,dev);
	if (sb)
		return sb;

	sb = alloc_sb();
	return sb;
}






static struct super_block* alloc_sb()
{
	static struct super_operations s_op;

	struct super_block* sb;
	sb = kmalloc(sizeof(struct super_block));

	if (sb) {
		memset(sb,0,sizeof(struct super_block));
		INIT_LIST_HEAD(&sb->s_list);
		sb->s_op = &s_op;
	}
	return sb;
}
