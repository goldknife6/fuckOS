#include <lib.h>
#include <stdio.h>

int main()
{
	int fd[2],ret,tty;
	tty = open("/dev/tty",0,0);
	ret = pipe(fd,0);
	printf("ret:%d fd0:%d fd1:%d\n",ret,fd[0],fd[1]);
	return 0;
}
