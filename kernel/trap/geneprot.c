#include <sys/system.h>
#include <fuckOS/assert.h>
#include <fuckOS/trap.h>
#include <fuckOS/task.h>
#include <fuckOS/sched.h>

extern int exit(struct task_struct *);

void geneprot_handler(struct frame *tf)
{
	print_frame(tf);
	exit(curtask);
	schedule();
	panic("geneprot not implemented!\n");
}
