#include <syscall.h>
#include <lib.h>


int write(uint32_t fd,char * buf,int count)
{
	return sys_write(fd,buf,count);
}

int writen(int fd, void *buf, int n)
{
	int m, tot;

	for (tot = 0; tot < n; tot += m) {
		m = write(fd, (char*)buf + tot, n - tot);
		if (m < 0)
			return m;
	}
	return tot;
}
