#include <lib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
char *msg = "this is tty write\n";
int main()
{
	int r,fd;
	char buf[512],c,*s;

	fd = open("/dev/tty",0,0);

	//s = readline(NULL);
	c = getchar();
	printf("this is %x %x\n" ,c,'\n');
	while(1);
	return 0;
}
