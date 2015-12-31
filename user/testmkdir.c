#include <lib.h>
#include <stdio.h>
#include <string.h>
char *dir = "/dev/";
char *file = "/dev/./file1";
char *file1 = "/dev/file1/../file2";
char *file2 = "/dev/file1/file2/file3";
char *file4 = "/dev/file2";
int main()
{
	int fd,retval;
	char buf[512];
	int i = 0;
	fd = mkdir(dir,0);
	printf("I am making a dirtory %s return value is %d\n",dir,fd);

	fd = mkdir(file,0);
	printf("I am making a dirtory %s return value is %d\n",file,fd);

	
	fd = mkdir(file1,0);
	printf("I am making a dirtory %s return value is %d\n",file1,fd);

	while(1);

	fd = create(file1,0);
	printf("I am creating a file %s return value is %d\n",file1,fd);

	fd = open(file1,0,0);
	printf("I am opening a file %s return value is %d\n",file1,fd);

	retval = read(fd,buf,512);
	printf("I am reading a file %s return value is %d\n",file1,retval);
	return 0;
}
