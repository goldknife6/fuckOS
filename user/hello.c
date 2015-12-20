#include <stdio.h>
#include <lib.h>
int main()
{
	int i = 0;
	while(1) {
			printf("hello!! pid:%d %d\n",getpid(),i++);	
	}
	return 0;
}
