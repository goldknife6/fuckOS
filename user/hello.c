#include <stdio.h>
#include <string.h>
#include <lib.h>

#include <fuckOS/list.h>

int main()
{
	int i = 0;
	void *p;
	for(;i < 2;i++ ) {
		p = malloc(0x2000000);
		printf("hello!! pid:%d %d %x\n",getpid(),i,p);
		memset(p,0,0x2000000);
	}


	return 0;
}
