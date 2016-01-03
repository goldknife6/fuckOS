#include <lib.h>
#include <stdio.h>
#include <fcntl.h>
int main()
{
	int fd[2],ret,tty,pid;
	char buf[512];
	tty = open("/dev/tty",O_RDWR,0);
	ret = pipe(fd,0);
	printf("ret:%d\n",ret);
	pid = fork();
	if (pid) {
		close(fd[0]);
		ret = write(fd[1],"this is write pipe!!!!",220);
		printf("write:%d\n",ret);
	} else {
		close(fd[1]);
		ret = readn(fd[0],buf,220);
		printf("readn:%d\n",ret);
		printf("read pipe:%.*s",ret,buf);
	}
	return 0;
}
