// buggy hello world -- unmapped pointer passed to kernel
// kernel should destroy user environment in response

#include <lib.h>


int main(int argc, char **argv)
{
	char *i = (char*)1;
	sys_cputs(i, 1);
	return 0;
}

