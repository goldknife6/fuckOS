#include <stdio.h>
#include <string.h>
#include <lib.h>

#include <fuckOS/list.h>
#define SIZE  0x100000
#define HIGH	8
void tree();

void alloc(int cur)
{
	if( cur == HIGH)
		return;

	pid_t pid = fork();
	if (pid == 0) {
		tree(cur + 1);
		char *p = malloc(SIZE);
		memset(p,0xFF,SIZE);
		printf("I am child [%d]\n",getpid());
	}
}

void tree(int cur)
{
	alloc(cur);
	alloc(cur);
}
int main()
{
	
	tree(0);

	return 0;
}
