#include <syscall.h>
#include <lib.h>



int wait(int pid)
{
	return sys_wait(pid);
}
