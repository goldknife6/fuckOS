#ifndef _MINIOS_NAMEI_H
#define _MINIOS_NAMEI_H
#include <types.h>

#include <fuckOS/fs.h>
#include <fuckOS/assert.h>
#define DNAME_LEN_MIN 36

#define LAST_NORM	0x1
#define LAST_DOT	0x2
#define LAST_DOTDOT	0x3

struct qstr {
	char name[DNAME_LEN_MIN];
	int len;
};

struct path {
	struct dentry *dentry;
	struct vfsmount *mnt;
};


struct nameidata {
	struct dentry *dentry;
	struct vfsmount *mnt;
	struct qstr last;
	uint32_t flags;
	int last_type;
};

enum {
	LOOKUP_FOLLOW = 1<<0,
	LOOKUP_DIRECTORY = 1<<1,
	LOOKUP_CONTINUE = 1<<2,
	LOOKUP_PARENT = 1<<3,
	LOOKUP_OPEN = 1<<4,
	LOOKUP_CREATE = 1<<5,
	LOOKUP_ACCESS = 1<<5,
};
static inline void print_qsrt(struct qstr *str)
{
	if (str)
		printk("%.*s %d\n",str->len,str->name,str->len);
}

static inline char* get_last(char *pathname,int len)
{
	char *p = pathname + len -1;
	while(*p == '/') p--;

	while (*p != '/' && p >= pathname) {
		p--;
	}
	
	return ++p;
}
#endif
