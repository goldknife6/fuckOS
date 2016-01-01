#include <syscall.h>
#include <lib.h>


int write(uint32_t fd,char * buf,int count)
{
	return sys_write(fd,buf,count);
}
