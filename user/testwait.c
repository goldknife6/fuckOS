#include <stdio.h>
#include <lib.h>
#include <fcntl.h>
int main()
{
	int fd;
	fd = open("/dev/tty",O_RDWR,0);
	pid_t pid = fork();
	if (pid>0) {
		printf("I am parent %d,My child's pid is %x\n",getpid(),pid);
		wait(pid);
		printf("child  %d exit %d\n",getpid(),pid);
	} else if (pid == 0){
		int i;
		printf("I am child ,My pid is %x\n",getpid());
		for(i = 0; i < 1000; i ++)
			getpid();

		printf("child exiting!\n");
		exit();
	} else {
		printf("fork error\n");
	}
	return 0;
}
