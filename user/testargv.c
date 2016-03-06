#include <lib.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	int fd,r;
	fd = open("/dev/tty",O_RDWR,0);

	printf("runing echo!\n");
	r = execve("/echo","echo1","echo","echo","echo");
	wait(r);
	printf("exit!\n");
	return 0;
}
