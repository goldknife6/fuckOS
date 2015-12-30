#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/fs.h>

#include <mm/pages.h>
#include <mm/slab.h>
#include <errno.h>
#include <string.h>
extern int alloc_pidmap();
extern void free_pidmap(pid_t);

extern int exit_task(struct task_struct *);
extern int exit_mm(struct mm_struct *);

static int copy_task(struct task_struct *,struct task_struct *);
static int alloc_task(struct task_struct **,pid_t);
int alloc_file(struct task_struct *);
static int task_set_vm(int,struct task_struct *);

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

	retval = task_set_vm(clone_flag,child);
	if (retval < 0)
		goto exit_task;

	if (clone_flag & CLONE_VM) {
		panic("clone_vm not imp! clone_flag:%x\n",clone_flag);
	} else {
		retval = copy_task(curtask,child);
		if (retval < 0)
			goto exit_mm;
	}
	schedule_add_task(child);
	task_pidmap[child->pid] = child;
	
	return child->pid;

exit_mm:
	exit_mm(child->mm);
exit_task:
	exit_task(child);
	return retval;

}

int alloc_task(struct task_struct **newenv_store,pid_t ppid)
{
	struct task_struct *task;
	task = kmalloc(sizeof(struct task_struct));
	if (!task) {
		return -ENOMEM;
	}

	task->pid = alloc_pidmap();
	if (task->pid < 0)
		return -EMPROC;

	task->ppid = ppid;
	task->task_type = TASK_TYPE_USER;
	task->task_status = TASK_RUNNING;


	memset(&task->frame, 0, sizeof(struct frame));

	if(newenv_store)
		*newenv_store  = task;
	return 0;
}

int alloc_file(struct task_struct *task)
{
	assert(task);
	task->files = kmalloc(sizeof(struct files_struct));
	if (!task->files)
		return -ENOMEM;

	task->fs = kmalloc(sizeof(struct fs_struct));
	if (!task->fs)
		return -ENOMEM;
	memset(task->files,0,sizeof(struct files_struct));
	memset(task->fs,0,sizeof(struct fs_struct));
	task->fs->pwd = root_dentry;
	task->fs->root = root_dentry;
	printk("task->fs:%x\n",task->fs);
	return 0;
}


static int task_set_vm(int clone_flag, struct task_struct *task)
{
	struct page *page = NULL;
	pgd_t *pgd;
	task->mm = alloc_mm();

	if (!task->mm)
		return -ENOMEM;
	task->mm->start_brk  = curtask->mm->start_brk;
	task->mm->end_brk  = curtask->mm->end_brk;
	task->mm->start_code  = curtask->mm->start_code;
	task->mm->end_code  = curtask->mm->end_code;
	task->mm->start_stack  = curtask->mm->start_stack;

	page = page_alloc(_GFP_ZERO);
	if (!page) {
		kfree(task->mm);
		return -ENOMEM;
	}
	pgd = (pgd_t *)page2virt(page);
	task->task_pgd = task->mm->mm_pgd = pgd;
	memmove(pgd,kpgd,PAGE_SIZE);
	return 0;
}

static int 
copy_task(struct task_struct *src,struct task_struct *dest)
{
	struct page *page;
	struct vm_area_struct* vma = src->mm->mmap;
	struct vm_area_struct* foo;
	int retval = 0;
	spin_lock(&src->mm->page_table_lock);
	while (vma) {
		viraddr_t start = vma->vm_start;

		viraddr_t end = vma->vm_end;	

		foo = create_vma(dest->mm, start, end - start, vma->vm_flags);
		if (!foo) {
			retval =  -ENOMEM;
			goto unlock;
		}
		for(;start < end; start += PAGE_SIZE) {

			pte_t *srcpte =  page_walk(src->mm->mm_pgd,start,0);

			if (!srcpte || !pte_present(*srcpte))
				continue;

			pte_t *destpte =  page_walk(dest->mm->mm_pgd,start,1);

			if (!destpte) {
				retval =  -ENOMEM;
				goto unlock;
			}
			
			page = virt2page(pte_page_vaddr(*srcpte));

			pte_clearwrite(srcpte);

			atomic_inc(&page->nref);

			*destpte = *srcpte;
		}

		vma = vma->vm_next;
	}
unlock:
	spin_unlock(&src->mm->page_table_lock);
	return retval;
}


