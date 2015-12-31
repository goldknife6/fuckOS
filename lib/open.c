#include <syscall.h>
#include <lib.h>
#include <string.h>
#include <stdio.h>
int open(char *filename,int flags,int mode)
{
	return sys_open(filename,strlen(filename),flags,mode);
}
