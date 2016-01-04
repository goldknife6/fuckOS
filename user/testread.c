#include <lib.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
	int fd,r;
	fd = open("/dev/tty",O_RDWR,0);
	while(1){
		r = getchar();
		printf("%x\n",r);
	}
	return 0;
}
