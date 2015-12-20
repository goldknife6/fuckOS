#include <fuckOS/assert.h>
#include <fuckOS/sched.h>
#include <fuckOS/kernel.h>
#include <fuckOS/cpu.h>
#include <fuckOS/list.h>

#include <mm/mmzone.h>

#include <asm/atomic.h>

struct runqueue runqueues[CPUNUMS];


void 
schedule_init()
{
	int id = get_cpuid();
	struct runqueue *rq = &runqueues[id];

	INIT_LIST_HEAD(&rq->head);
}

void 
schedule_add_task(struct task_struct *task)
{
	list_add_tail(&task->list,&thisrq->head);
}

void 
schedule_delete_task(struct task_struct *task)
{
	list_del(&task->list);
}

void 
schedule_tick()
{
	
}
static struct task_struct *
get_next()
{
	struct task_struct * task;

	if(list_empty(&thisrq->head))
		panic("runqueues is empty! cpu:%d\n",get_cpuid());

	task = list_entry(thisrq->head.next,struct task_struct,list);

	return task;
}
void 
schedule()
{
	struct task_struct *next = get_next();
	task_run(next);
}
