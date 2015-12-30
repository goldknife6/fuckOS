#include <lib.h>
#include <stdio.h>

int main()
{
	int fd,retval;
	char buf[512];
	fd = create("/root",0);
	printf("I am user I am creatning root %d\n",fd);

	fd = open("/root12",0,0);
	printf("I am user I am openning root %d\n",fd);

	retval = read(fd,buf,512);
	printf("I am reading,%d bytes, content:%s\n",retval,buf);
	return 0;
}
