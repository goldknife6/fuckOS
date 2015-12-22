#include <syscall.h>
#include <lib.h>


void
exit()
{
	sys_exit(0);
}
