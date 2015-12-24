// program to cause a general protection exception

#include <stdio.h>


int main(int argc, char **argv)
{
	// Try to load the kernel's TSS selector into the DS register.
	asm volatile("movw $0x28,%ax; movw %ax,%ds");
	return 0;
}

