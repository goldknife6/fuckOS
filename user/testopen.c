#include <lib.h>
#include <stdio.h>
#include <fcntl.h>
int main()
{
	int fd,retval;
	char buf[512];
	fd = open("/dev/tty",O_RDWR,0);

	fd = open("/shell_sh",O_RDWR,0);
	printf("I am user I am openning fd is %d\n",fd);

	retval = read(fd,buf,512);

	printf("I am reading,%d bytes, content:%.*s\n",retval,retval,buf);
	return 0;
}
