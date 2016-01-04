#include <lib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int
main(int argc, char **argv)
{
	char buf[100];
	int i, r, pid, p[2];


	int fd;
	fd = open("/dev/tty",O_RDWR,0);
	pid = fork();
	if (!pid) {
		r = execve("/hello",0,0);
		printf("r=%d\n",r);
	} else {
		printf("pid=%d\n",pid);
	}
	return 0;
}
