#include <lib.h>
#include <stdio.h>
#include <string.h>
char *msg = "this is tty write\n";
int main()
{
	int r,fd;
	char buf[512];

	fd = open("/dev/tty",0,0);

	r = write(fd,msg,strlen(msg));
	//printf("%d %s\n",r,buf);
	return 0;
}
