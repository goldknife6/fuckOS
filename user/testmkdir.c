#include <lib.h>
#include <stdio.h>
#include <string.h>
char *dir = "/dev";
char *file = "/dev/../././../../../../dev";
int main()
{
	int fd,retval;
	char buf[512];
	int i = 0;
	fd = create(dir,0);
	printf("I am create a file %s return value is %d\n",dir,fd);

	fd = open(file,0,0);
	printf("I am open a file %s return value is %d\n",file,fd);

	return 0;
}
