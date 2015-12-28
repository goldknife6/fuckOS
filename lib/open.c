#include <syscall.h>
#include <lib.h>


int open(char *filename,int flags,int mode)
{
	return sys_open(filename,flags,mode);
}
