#include <syscall.h>
#include <lib.h>
#include <string.h>

int mkdir(char *filename,int mode)
{
	return sys_mkdir(filename,strlen(filename),mode);
}
