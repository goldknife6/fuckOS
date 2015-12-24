// evil hello world -- kernel pointer passed to kernel
// kernel should destroy user environment in response

#include <lib.h>


int main(int argc, char **argv)
{
	// try to print the kernel entry point as a string!  mua ha ha!
	sys_cputs((char*)0xc010000c, 100);
	return 0;
}

