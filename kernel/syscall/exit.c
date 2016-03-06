#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/assert.h>

#include <mm/mm.h>
#include <mm/slab.h>

extern int exit_mm(struct mm_struct *);
extern int exit_task(struct task_struct *);
extern int exit_notify(struct task_struct *);
extern int exit_files(struct task_struct *);

int exit(struct task_struct *task)
{
	assert(task);

	if (task == curtask)
		lcr3(pgd2p(kpgd));
	
	schedule_delete_task(task);
	exit_notify(task);
	exit_mm(task->mm);
	exit_files(task);
	exit_task(task);

	if (task == curtask) {
		curtask = NULL;
		schedule();
	}
	return 0;
}

int exit_notify(struct task_struct *t)
{
	struct task_struct *p;
	int ret;
	if (!t)
		return 0;
	

	if (t->pwait) {
		p = task_pidmap[t->ppid];
		//ret = pid2task(t->ppid, &p, 0);
		//assert(ret == 0);
		p->task_status = TASK_RUNNING;
		schedule_add_task(p);
	}
	return 0;
}

int exit_files(struct task_struct *task)
{
	int i;
	for (i = 0; i < MAX_FILE;i++) {
		deref_file(i);	
	}
	return 0;
}

static int 
delete_all_vma(struct mm_struct* mm)
{
	if (!mm) return 0;

	struct vm_area_struct* vma = mm->mmap;
	if (!vma) return 0;

	while (vma) {
		delete_vma(vma);
		vma = mm->mmap;
	}
	mm->mmap = NULL;
	return 0;
}
int exit_mm(struct mm_struct *mm)
{
	pmd_t *pmd;
	pgd_t *pgd;
	uint32_t pgdno, pmdno;
	physaddr_t pa;

	struct vm_area_struct* vma = mm->mmap;
	struct page *page;
	
	if(!mm || !mm->mm_pgd)
		return 0;

	if(!atomic_dec_and_test(&mm->mm_count))
		return 0;

	delete_all_vma(mm);

	for (pgdno = 0; pgdno < pgd_index(KERNEL_BASE_ADDR); pgdno++) {
		pgd = mm->mm_pgd + pgdno;
		if(!pgd_present(*pgd) || pgd_none(*pgd))
			continue;
		pmd_t* tmp = (pmd_t *)pgd_page_vaddr(*pgd);
		
		for (pmdno = 0; pmdno < PTRS_PER_PMD; pmdno++) {
			pmd = tmp +  pmdno;
			if(!pmd_present(*pmd) || pmd_none(*pmd))
				continue;
			struct page* p = virt2page(pmd_page_vaddr(*pmd));
			page_decref(p);
			pmd_set(pmd,0,0);
		}
		struct page* p = virt2page(pgd_page_vaddr(*pgd));
		page_decref(p);
		pgd_set(pgd,0,0);
	}

	page = virt2page((viraddr_t)mm->mm_pgd);
	page_free(page);
	kfree(mm);

	return 0;
}

int exit_task(struct task_struct *task)
{
	if (!task)
		return 0;

	if (task->pid >= 0) {
		task_pidmap[task->pid] = NULL;
		free_pidmap(task->pid);
	}
	kfree(task);
	return 0;
}
