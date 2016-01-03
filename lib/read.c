#include <syscall.h>
#include <lib.h>
#include <errno.h>

int read(uint32_t fd,void * buf,int count)
{
	return sys_read(fd,buf,count);
}

int readn(int fd, void *buf, int n)
{
	int m, tot;

	for (tot = 0; tot < n; tot += m) {
		m = read(fd, (char*)buf + tot, n - tot);
		if (m < 0) {
			return m;
		}
		if (m == 0)
			return tot;
	}
	return tot;
}
