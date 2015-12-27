#include <lib.h>
#include <stdio.h>

int main()
{
	int r;
	char buf[512];
	r = read(1,buf,512);
	printf("%d %s\n",r,buf);
	return 0;
}
