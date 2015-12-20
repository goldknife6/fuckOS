#include <sys/system.h>
#include <fuckOS/assert.h>
#include <fuckOS/trap.h>

int syscall_handler(struct frame *tf)
{
	print_frame(tf);
	panic("syscall not implemented!\n");

	return 0;
}
