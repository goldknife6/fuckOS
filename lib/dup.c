#include <syscall.h>
#include <lib.h>



int dup(int fd)
{
	return sys_dup(fd);
}
