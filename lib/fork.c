#include <syscall.h>
#include <lib.h>


pid_t
fork()
{
	return sys_clone(0,NULL);
}
