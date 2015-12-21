#ifndef _LIB_H
#define _LIB_H


#include <syscall.h>
#include <fuckOS/list.h>
extern void sys_cputs(const char *s, size_t len);
extern int sys_exit(pid_t pid);
extern pid_t sys_exofork(void);
extern pid_t sys_getpid(void);
extern int sys_brk(viraddr_t);

extern viraddr_t brk(viraddr_t);
extern viraddr_t sbrk(viraddr_t);


extern pid_t fork(void);
extern void exit(void);
extern pid_t getpid(void);
extern void* malloc(uint32_t);
extern void free(void *);



struct malloc_chunk {
	uint32_t size;
	int flag;
	struct list_head list;
};







#endif/*_LIB_H*/
