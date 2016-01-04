#include <syscall.h>
#include <lib.h>
#include <string.h>
#include <sys/stat.h>
int fdtype(int fd)
{
	return sys_fdtype(fd);
}

int iscons(int fd)
{
	int type = fdtype(fd);
	if (S_IFCHR==type)
		return 1;

	return 0;
}
