#include <types.h>
#include <fuckOS/task.h>
#include <fuckOS/sched.h>

int wait(pid_t pid)
{
	struct task_struct* task;

	if (pid > PID_MAX_DEFAULT || pid < 0)
		return -EINVAL;
	
	task = task_pidmap[pid];
	if (!task || (task->ppid != curtask->pid)) {
		return -EBADP;
	}

	task->pwait = 1;

	curtask->task_status = TASK_INTERRUPTIBLE;
	schedule_delete_task(curtask);
	schedule();
	return 0;
}
