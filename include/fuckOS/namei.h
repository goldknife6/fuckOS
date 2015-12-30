#ifndef _MINIOS_NAMEI_H
#define _MINIOS_NAMEI_H
#include <types.h>

#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#define DNAME_LEN_MIN 36

struct qstr {
	char name[DNAME_LEN_MIN];
	int len;
};


struct nameidata {
	struct dentry *dentry;
	struct vfsmount *mnt;
	struct qstr last;
	uint32_t flags;
	int last_type;
};

static inline void print_qsrt(struct qstr *str)
{
	if (str)
		printk("%.*s ",str->len,str->name);
}

#endif
