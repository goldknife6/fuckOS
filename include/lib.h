#ifndef _LIB_H
#define _LIB_H


#include <syscall.h>
#include <fuckOS/list.h>
extern void sys_cputs(const char *s, size_t len);
extern int sys_exit(pid_t pid);
extern pid_t sys_clone(int ,int (*)(void*));
extern pid_t sys_getpid(void);
extern int sys_brk(viraddr_t);
extern int sys_read(uint32_t fd,char * buf,int count);
extern int sys_open(char *filename,int flags,int mode);

extern viraddr_t brk(viraddr_t);
extern viraddr_t sbrk(viraddr_t);


extern pid_t fork(void);
extern void exit(void);
extern pid_t getpid(void);
extern void* malloc(uint32_t);
extern void free(void *);
extern int read(uint32_t fd,char * buf,int count);
extern int open(char *filename,int flags,int mode);

struct malloc_chunk {
	uint32_t size;
	int flag;
	struct list_head list;
};







#endif/*_LIB_H*/
