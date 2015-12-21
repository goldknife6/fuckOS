#include <types.h>

#include <mm/mm.h>
#include <mm/pages.h>

viraddr_t brk(viraddr_t end_data)
{

	viraddr_t va = 0;
	struct mm_struct* mm;
	struct vm_area_struct *vma;
	struct vm_area_struct *vma_next;

	mm = curtask->mm;

	if (end_data <= mm->end_brk)
		return mm->end_brk;

	spin_lock(&mm->page_table_lock);

	vma = find_vma(mm, mm->start_brk);

	if(!vma || vma->vm_start != mm->start_brk) {
		printk("%x %x\n",vma->vm_start,mm->start_brk);
		panic("brk!\n");
	}

	if (end_data <= vma->vm_start)
		return vma->vm_start;

	vma_next = vma->vm_next;

	if(!vma_next || vma_next->vm_start >= end_data) {
		mm->end_brk = end_data;
		vma->vm_end = end_data;
		va = end_data;
	}

	spin_unlock(&mm->page_table_lock);
	return va;
}
