#include <lib.h>
#include <stdio.h>
#include <string.h>
char *dir = "/dev";
char *file = "/dev/tty12";
int main()
{
	int fd,retval;
	char buf[512];
	int i = 0;

	fd = open("/dev/tty",0,0);
	printf("I am open a file %s return value is %d\n","/dev/tty",fd);

	return 0;
}
