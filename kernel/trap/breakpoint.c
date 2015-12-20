#include <sys/system.h>
#include <fuckOS/assert.h>
#include <fuckOS/trap.h>
void breakpoint_handler(struct frame *tf)
{
	print_frame(tf);
	panic("breakpoint not implemented!\n");
}
