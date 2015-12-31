#include <lib.h>
#include <stdio.h>
#include <string.h>
int main()
{
	int fd,retval;
	char buf[512];
	char name[32];
	int i = 0;
	for (i; i < 10; i++){
		snprintf(name, 32, "read%d", i);
		fd = create(name,0);
		printf("I am user I am creatning %s %d\n",name,fd);
	}
	fd = open("/root",0,0);
	printf("I am user I am openning root %d\n",fd);

	retval = read(fd,buf,512);
	printf("retval %d\n",retval);

	return 0;
}
