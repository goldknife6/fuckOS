#include <lib.h>
#include <stdio.h>
#include <string.h>

char *buf = "test close!!\n";
int main()
{
	int r,fd1,fd2,i = 0;
	
	fd1 = open("/dev/tty",0,0);
	r = fork();
	if (r == 0) {
		printf("I am child! pid:%d i:%d\n",getpid(),i++);
	} else {
		printf("I am parent! pid:%d\n",getpid());
		close(fd1);while(1);
	}
	printf("I am  pid:%d\n",getpid());
	return 0;
}
