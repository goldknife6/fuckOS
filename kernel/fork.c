#include <fuckOS/task.h>

#include <mm/slab.h>
static int copy_mm(int flags,struct task_struct *child);

int do_fork(int clone_flags, void* kstack)
{
	int retval = -ENOMEM;
	struct task_struct *task = NULL;

	task = alloc_task();
	if (!task)
		goto out_fork;


	retval = copy_mm(clone_flags,task);

	if(retval < 0) {
		goto bad_fork_cleanup_mm;
	}
	
	
bad_fork_cleanup_mm:
	//exit_mm(task);
out_fork:
	return retval;
}

static int copy_mm(int flags,struct task_struct *child)
{

	if (flags & CLONE_VM) {
		spin_lock(&curtask->mm->page_table_lock);
		
		child->mm = curtask->mm;
		child->task_pgd = curtask->task_pgd;
		child->mm->mm_count++;

		spin_unlock(&curtask->mm->page_table_lock);
	} else if (flags & CLONE_KTHREAD) {
		//
	} else {
		child->mm = alloc_mm();
		if (!child->mm)
			return -ENOMEM;
	}
	return 0;
}



