#ifndef _MINIOS_SCHED_H
#define _MINIOS_SCHED_H

#include <fuckOS/rbtree.h>
#include <fuckOS/list.h>
#include <fuckOS/task.h>
#include <fuckOS/cpu.h>

struct runqueue {
	struct list_head head;
};


extern struct runqueue runqueues[];

extern void schedule(void);
extern void schedule_tick(void);
extern void schedule_init();
extern void schedule_delete_task(struct task_struct *);
extern void schedule_add_task(struct task_struct *);



#define thisrq (&runqueues[get_cpuid()])

#endif/*_MINIOS_SCHED_H*/
