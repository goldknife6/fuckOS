#include <syscall.h>
#include <lib.h>


int read(uint32_t fd,void * buf,int count)
{
	return sys_read(fd,buf,count);
}
