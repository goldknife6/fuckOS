#include <syscall.h>
#include <lib.h>



int dup(int fd)
{
	return sys_dup(fd);
}

int dup2(int oldfd,int newfd)
{
	return sys_dup2(oldfd,newfd);
}
