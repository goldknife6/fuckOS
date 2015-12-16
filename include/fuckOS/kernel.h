#ifndef _fuckOS_KERNEL_H
#define _fuckOS_KERNEL_H
#include <stdarg.h>
#include <types.h>

#ifdef		CONFIG_PAE
# define	PLEN	"016ll"
#else
# define	PLEN	"08l"
#endif

extern int printk(const char *fmt, ...);

#endif/*_fuckOS_KERNEL_H*/

