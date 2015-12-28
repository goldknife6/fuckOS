#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <types.h>
/* system call numbers */
enum {
	SYS_CPUTS = 0,
	SYS_EXIT,
	SYS_EXOFORK,
	SYS_GETPID,
	SYS_BRK,
	SYS_READ,
	SYS_OPEN,
	NSYSCALLS
};

#endif /* !_SYSCALL_H */
