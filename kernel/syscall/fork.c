#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <mm/pages.h>
#include <mm/slab.h>
#include <errno.h>
#include <string.h>
extern int alloc_pidmap();
extern void free_pidmap(pid_t);


static void copy_task(struct task_struct *,struct task_struct *);
static int alloc_task(struct task_struct **,pid_t);

pid_t do_fork(int clone_flag,struct frame frame)
{
	int retval;
	struct task_struct *child = NULL;
	retval = alloc_task(&child,curtask->pid);
	if (retval < 0) {
		goto exit_task;
	}

	//copy trap frame
	child->frame = frame;
	child->frame.tf_regs.reg_eax = 0;

	retval = task_set_vm(child);
	if (retval < 0) {
		goto exit_mm;
	}

	if (clone_flag & CLONE_VM) {
		panic("clone_vm not imp! clone_flag:%x\n",clone_flag);
	} else {
		copy_task(curtask,child);
	}
	schedule_add_task(child);
	return child->pid;

exit_mm:
	free_mm(child->mm);

exit_task:
	return retval;

}

static int alloc_task(struct task_struct **newenv_store,pid_t ppid)
{
	struct task_struct *task;
	task = kmalloc(sizeof(struct task_struct));
	if (!task) {
		return -ENOMEM;
	}

	task->pid = alloc_pidmap();
	if (task->pid < 0)
		return -EMPROC;
	// Set the basic status variables.
	task->ppid = ppid;
	task->task_type = TASK_TYPE_USER;
	task->task_status = TASK_RUNNING;


	memset(&task->frame, 0, sizeof(struct frame));

	if(newenv_store)
		*newenv_store  = task;
	return 0;
}

static void 
copy_task(struct task_struct *src,struct task_struct *dest)
{
	struct vm_area_struct* vma = src->mm->mmap;
	spin_lock(&src->mm->page_table_lock);
	while (vma) {
		viraddr_t start = vma->vm_start;

		viraddr_t end = vma->vm_end;

		create_vma(dest->mm, start, end - start, vma->vm_flags);

		for(;start < end; start += PAGE_SIZE) {

			pte_t *srcpte =  page_walk(src->mm->mm_pgd,start,0);

			if (!srcpte || pte_none(*srcpte))
				continue;

			pte_clearwrite(srcpte);

			struct page *page = virt2page(pte_page_vaddr(*srcpte));

			atomic_inc(&page->nref);

			pte_t *destpte =  page_walk(dest->mm->mm_pgd,start,1);
			assert(destpte);
			*destpte = *srcpte;
		}

		vma = vma->vm_next;
	}
	spin_unlock(&src->mm->page_table_lock);
}


