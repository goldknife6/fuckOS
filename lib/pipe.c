#include <syscall.h>
#include <lib.h>



int pipe(int fd[2],int flags)
{
	return sys_pipe(fd,flags);
}
