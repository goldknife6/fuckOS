#include <lib.h>
#include <stdio.h>

int main()
{
	int fd,retval;
	char buf[512];
	fd = open("/",0,0);
	printf("I am user I am openning fd is %d\n",fd);

	retval = read(fd,buf,512);

	printf("I am reading,%d bytes, content:%s\n",retval,buf);
	return 0;
}
