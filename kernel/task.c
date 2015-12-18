#include <fuckOS/task.h>

#include <mm/slab.h>

struct task_struct *init_task;

void task_init()
{
	init_task = kmalloc(sizeof(struct task_struct));
	assert(init_task);
	init_task->mm = kmalloc(sizeof(struct mm_struct));
	assert(init_task->mm);
	init_task->mm->mm_pgd = kpgd;
}
