// Fork a binary tree of processes and display their structure.

#include <lib.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
	open("/dev/tty",O_RDWR,0);
	
	while(1)
		;//printf("idle!%d\n",getpid());
	return 0;
}
