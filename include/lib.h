#ifndef _LIB_H
#define _LIB_H


#include <syscall.h>

extern void sys_cputs(const char *s, size_t len);
extern int sys_exit(pid_t pid);
extern pid_t sys_exofork(void);
extern pid_t sys_getpid(void);

extern pid_t fork(void);
extern void exit(void);
extern pid_t getpid(void);
#endif/*_LIB_H*/
