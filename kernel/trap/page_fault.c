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
#include <fuckOS/kernel.h>



static int handle_pte_fault(struct mm_struct *, struct vm_area_struct *, viraddr_t , pte_t *,uint32_t );
static int do_wp_fault(struct mm_struct *,struct vm_area_struct *,viraddr_t ,pte_t *);



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
	spin_lock(&curtask->mm->page_table_lock);

	vma = find_vma(curtask->mm, va);

	if (!vma || vma->vm_start > va) {
		panic("page_fault exit:%x\n",va);
		return;
	}
	
	pte = page_walk(curtask->mm->mm_pgd,va,true);
	
	if(!pte) {
		panic("page_fault !pte exit\n");
	}

	handle_pte_fault(curtask->mm, vma, va, pte, tf->tf_err);

	spin_unlock(&curtask->mm->page_table_lock);
}

static int 
handle_pte_fault(struct mm_struct *mm, struct vm_area_struct *vma, 
				viraddr_t address, pte_t *pte,uint32_t err)
{
	int write_access = err & 2;
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
			struct page *new = page_alloc(_GFP_ZERO);
			if (!new) {
				panic("extend STACK!\n");
			}
			page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

		} else if (vma->vm_start == mm->start_brk) {
			struct page *new = page_alloc(_GFP_ZERO);
			if (!new) {
				panic("extend brk!\n");
			}
			page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );
		}
	}
	return 0;
}

static int 
do_wp_fault(struct mm_struct *mm,
	struct vm_area_struct *vma,viraddr_t address,pte_t *pte)

{
	struct page *page = virt2page(pte_page_vaddr(*pte));

	if (atomic_read(&page->nref) == 1) {
		pte_mkwrite(pte);
	} else {
		struct page *new = page_alloc(_GFP_ZERO);

		assert(new);

		page_insert(mm->mm_pgd, new,USER_TEMPBOTT, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

		memcpy((void*)USER_TEMPBOTT, (void*)ROUNDDOWN(address,PAGE_SIZE), PAGE_SIZE);

		page_insert(mm->mm_pgd, new,address, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER );

		page_remove(mm->mm_pgd, USER_TEMPBOTT);
	}
	return 0;
}
