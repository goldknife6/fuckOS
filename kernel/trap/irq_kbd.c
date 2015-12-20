#include <fuckOS/assert.h>
#include <fuckOS/trap.h>




void kbd_handler(struct frame *tf)
{
	print_frame(tf);
	panic("kbd not implemented!\n");
}
