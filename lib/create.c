#include <syscall.h>
#include <lib.h>
#include <string.h>

int create(char *filename,int mode)
{
	return sys_create(filename,strlen(filename),mode);
}
