#include <fuckOS/assert.h>
#include <fuckOS/sched.h>
#include <fuckOS/kernel.h>
#include <fuckOS/cpu.h>
#include <mm/mmzone.h>

#include <asm/atomic.h>

struct runqueue runqueues[CPUNUMS];


void 
schedule_init(struct task_struct *idle)
{
	
}




void 
schedule_add_task(struct task_struct *task)
{
}

void 
schedule_delete_task(struct task_struct *task)
{
}



struct task_struct *
get_next_task()
{
		
}

void 
schedule_tick()
{
	
}

void 
schedule()
{
	
}
