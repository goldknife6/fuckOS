#include <lib.h>
#include <stdio.h>
#include <string.h>
void fun(int i) {
	int esp;
	char buf[10];
	asm("movl %%esp,%0":"=m"(esp));
	printf("esp %x %d\n",esp,i);
	memset(buf,0,10);
	fun(i + 1);
}
int main()
{
	fun(0);
	return 0;
}
