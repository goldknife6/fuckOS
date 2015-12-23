#include <sys/system.h>

#include <asm/x86.h>
#include <asm/atomic.h>

#include <mm/mm.h>
#include <mm/pages.h>

#include <types.h>
#include <string.h>

#include <fuckOS/assert.h>
#include <fuckOS/trap.h>
#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/kernel.h>



static int handle_pte_fault(struct mm_struct *, struct vm_area_struct *, viraddr_t , pte_t *,uint32_t );
static int do_wp_fault(struct mm_struct *,struct vm_area_struct *,viraddr_t ,pte_t *);
extern int exit(struct task_struct *);


void 
vma_debug(struct vm_area_struct* vma)
{
	assert(vma);
	printk("vma:%08x vm_start:0x%08x vm_end:0x%08x\n",vma,vma->vm_start,vma->vm_end);
}

void page_fault_handler(struct frame *tf)
{
	viraddr_t va;
	struct vm_area_struct *vma;
	pte_t *pte;

	va  = rcr2();
	//spin_lock(&curtask->mm->page_table_lock);

	vma = find_vma(curtask->mm, va);

	if (!vma || vma->vm_start > va) {
		//print_frame(tf);
		panic("page_fault exit va:%x pid:%d eip:%x\n",va,curtask->pid,tf->tf_eip);
		return;
	}
	
	pte = page_walk(curtask->task_pgd,va,true);
	
	if(!pte) {
		exit(curtask);
		schedule();
		panic("page_fault !pte exit\n");
	}

	handle_pte_fault(curtask->mm, vma, va, pte, tf->tf_err);

	//spin_unlock(&curtask->mm->page_table_lock);
}

static int 
handle_pte_fault(struct mm_struct *mm, struct vm_area_struct *vma, 
				viraddr_t address, pte_t *pte,uint32_t err)
{
	int retval;
	int write_access = err & 2;
	struct page *new = NULL;
	assert(pte);
	assert(vma);
	assert(mm);

	if (pte_present(*pte)) {
		if (write_access) {
			if (!pte_write(*pte)) {
				return do_wp_fault(mm,vma,address,pte);
			} else {
				panic("write_access !\n");
			}
		} else {
			panic("read_access !\n");
		}
	} else {
		if (vma->vm_start == USER_STACKBOTT) {
			new = page_alloc(_GFP_ZERO);
			if (!new) {
				goto exit;
			}
			retval = page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

			if (retval < 0) {
				panic("page_insert 1!\n");
				goto exit;
			}
		} else if (vma->vm_start == mm->start_brk) {
			new = page_alloc(_GFP_ZERO);
			if (!new)
				panic("extend brk!\n");
			retval = page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

			if (retval < 0) {
				panic("page_insert 2!\n");
				goto exit;
			}
		} else {
			panic("extend ?!%x\n",address);
			goto exit;
		}
	}
	return 0;

exit:
	if (new) {
		page_free(new);
	}
	exit(curtask);
	schedule();
	return 0;
}
static int 
do_wp_fault(struct mm_struct *mm,
	struct vm_area_struct *vma,viraddr_t address,pte_t *pte)
{
	struct page *page = virt2page(pte_page_vaddr(*pte));
	struct page *new = NULL;
	int retval;
	if (atomic_read(&page->nref) == 1) {
		
		pte_mkwrite(pte);
	} else {
		new = page_alloc(_GFP_ZERO);
		if (!new) {
			goto exit;
		}
		
		retval = page_insert(mm->mm_pgd, new,USER_TEMPBOTT, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );
		if (retval < 0) {
			goto exit;
		}
		
		memmove((void*)USER_TEMPBOTT, (void*)ROUNDDOWN(address,PAGE_SIZE), PAGE_SIZE);
		
		retval = page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

		page_remove(mm->mm_pgd, USER_TEMPBOTT);

		if (retval < 0) {
			panic("page_insert 4!\n");
			goto exit;
		}
		
	}
	return 0;

exit:
	if (new) {
		page_free(new);
	}
	exit(curtask);
	schedule();
	return 0;
}
