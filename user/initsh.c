#include <lib.h>
#include <stdio.h>
#include <fcntl.h>

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

void _panic(char *file,int len,...) {
		panic("panic:%s %d",file,len);
		while(1);
}

int
main(int argc, char **argv)
{
	int i, r, x, want;

	// being run directly from kernel, so no file descriptors open yet
	close(0);
	if ((r = open("/dev/tty",O_RDWR,0)) < 0)
		panic("opencons: %e", r);


	if (r != 0)
		panic("first opencons used fd %d", r);
	if ((r = dup2(0, 1)) < 0)
		panic("dup: %e", r);

	//printf("initsh: running sh\n");

	while (1) {
		printf("starting sh\n");
		r = execve("/sh", "sh", (char*)0);
		if (r < 0) {
			printf("init: spawn sh: %e\n", r);
			continue;
		}
		wait(r);
	}
	return 0;
}
