// Fork a binary tree of processes and display their structure.

#include <lib.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd;
	fd = open("/dev/tty",O_RDWR,0);
	
	
	printf("idle!%d\n",iscons(fd));
	return 0;
}
