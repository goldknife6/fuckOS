#include <syscall.h>
#include <lib.h>


int read(uint32_t fd,char * buf,int count)
{
	return sys_read(fd,buf,count);
}
