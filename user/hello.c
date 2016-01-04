#include <stdio.h>
#include <lib.h>
#include <fcntl.h>

int main()
{
	printf("Hello World! I am task %d\n",getpid());
	return 0;
}
