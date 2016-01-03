#include <lib.h>
#include <stdio.h>
#include <string.h>

char *buf = "test dup!!\n";
int main()
{
	int r,fd;
	
	fd = open("/dev/tty",0,0);
	fd = dup(fd);
	
	printf("fd:%d\n",fd);

	write(fd,buf,strlen(buf));

	

	return 0;
}
