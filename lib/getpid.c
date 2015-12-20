#include <syscall.h>
#include <lib.h>


pid_t
getpid()
{
	return sys_getpid();
}
