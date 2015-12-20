#include <sys/system.h>
#include <fuckOS/assert.h>
#include <fuckOS/trap.h>




void page_fault_handler(struct frame *tf)
{
	print_frame(tf);
	panic("page_fault not implemented!\n");
}
