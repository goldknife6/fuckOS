#include <stdio.h>
#include <lib.h>

int main()
{
	pid_t pid = fork();
	if (pid>0) {
		printf("I am parent %d,My child's pid is %x\n",getpid(),pid);
	} else if (pid == 0){
		printf("I am child ,My pid is %x\n",getpid());
	} else {
		printf("fork error\n");
	}
	return 0;
}
