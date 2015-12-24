// buggy hello world 2 -- pointed-to region extends into unmapped memory
// kernel should destroy user environment in response

#include <lib.h>

const char *hello = "hello, world\n";


int main(int argc, char **argv)
{
	sys_cputs(hello, 1024*1024);
	return 0;
}

