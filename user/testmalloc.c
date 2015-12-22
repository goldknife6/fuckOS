#include <stdio.h>
#include <string.h>
#include <lib.h>

#include <fuckOS/list.h>
void *arr[200000]; 
int main()
{
	int i = 0;
	void *p;
	for(;i < 200000;i++ ) {
		p = malloc(0x100);
		//printf("hello!! pid:%d %d %x\n",getpid(),i,p);
		memset(p,0,0x100);
		arr[i] = p;
	}

	for(i = 0;i < 200000;i++ ) {
		free(arr[i]);
		//printf("free!! pid:%d %d \n",getpid(),i);
	}
 printf("free!! pid:%d %d \n",getpid(),i);
	return 0;
}
