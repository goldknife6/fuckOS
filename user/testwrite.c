#include <lib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
char *msg = "this is tty write\n";
int main()
{
	int r,fd;
	char buf[512],c,*s;

	fd = open("/dev/tty1",0,0);
	if (fd < 0)
		return -1;

	printf("open fd is %d \n" ,fd);
	s = readline(NULL);
	printf("this is %s %x\n" ,s,'\n');
	while(1);
	return 0;
}
