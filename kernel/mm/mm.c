#include <errno.h>
#include <string.h>

#include <mm/mm.h>
#include <mm/slab.h>
#include <mm/pages.h>
#include <mm/layout.h>

#include <fuckOS/task.h>

#include <asm/atomic.h>

struct mm_struct* alloc_mm()
{
	struct mm_struct* mm;
	mm = kmalloc(sizeof(struct mm_struct));

	if(!mm)
		return NULL;
	memset(mm,0,sizeof(struct mm_struct));

	mm->mmap = NULL;
	RBTREE_ROOT_INIT(&mm->mm_rb);
	mm->mm_pgd = NULL;
	mm->free_area_cache = USER_UNNAME_ZONE;
	return mm;
}
struct vm_area_struct*
find_vma(struct mm_struct* mm, viraddr_t addr)
{
	struct rb_node* rb_node = mm->mm_rb.rb_node;
	struct vm_area_struct* vma = NULL;
	struct vm_area_struct* vma_tmp = NULL;

	while (rb_node) {

		vma_tmp = rb_entry(rb_node, struct vm_area_struct, vm_rb);

		if (vma_tmp->vm_end > addr) {

			vma = vma_tmp;

			if(vma_tmp->vm_start <= addr)
				break;

			rb_node = rb_node->rb_left;		
		} else
			rb_node = rb_node->rb_right;
	}
	return vma;
}

struct vm_area_struct*
find_vma_prev(struct mm_struct* mm, viraddr_t addr)
{
	struct rb_node* rb_node = mm->mm_rb.rb_node;
	struct vm_area_struct* vma = NULL;
	struct vm_area_struct* vma_tmp = NULL;

	while (rb_node) {
		vma_tmp = rb_entry(rb_node, struct vm_area_struct, vm_rb);

		if (vma_tmp->vm_start <= addr) {

			vma = vma_tmp;

			if(vma_tmp->vm_end > addr)
				break;

			rb_node = rb_node->rb_right;		
		} else
			rb_node = rb_node->rb_left;
	}
	return vma;
}

viraddr_t 
get_unmapped_vma(struct mm_struct* mm, viraddr_t addr, size_t len)
{
	struct vm_area_struct* vma = NULL;

	if(len > KERNEL_BASE_ADDR)
		return 0;

	spin_lock(&mm->page_table_lock);
	
	if (addr) {
		vma = find_vma(mm, addr);
		if (KERNEL_BASE_ADDR  >= addr + len &&
		    (!vma || addr + len <= vma->vm_start))
			goto ok;
		else
			goto out;
	} else {
		viraddr_t start_addr = addr = mm->free_area_cache;

		for (vma = find_vma(mm, addr); ; vma = vma->vm_next) {

			if (addr + len > KERNEL_BASE_ADDR) {
				if (start_addr == 0x40000000)
					goto out;
				start_addr = addr = 0x40000000;

				vma = find_vma(mm, addr);
			}

			if (!vma || addr + len <= vma->vm_start) {
				mm->free_area_cache = addr + len;
				goto ok;
			}
			addr = vma->vm_end;
		}
	}
out:
	spin_unlock(&mm->page_table_lock);
	return 0;
ok:	
	spin_unlock(&mm->page_table_lock);
	return addr;
}


static void 
vm_link(struct mm_struct* mm, struct vm_area_struct* vmp)
{
	viraddr_t addr = vmp->vm_end;
	struct rb_node** rb_node = &mm->mm_rb.rb_node;
	struct rb_node* parent = NULL;
	struct vm_area_struct* vma;
	
	spin_lock(&mm->page_table_lock);

	vma = find_vma_prev(mm, vmp->vm_start);
	if (!vma) {
		vmp->vm_next = mm->mmap;
		mm->mmap = vmp;
	} else {
		vmp->vm_next = vma->vm_next;
		vma->vm_next = vmp;
	}

	while (*rb_node) {
		parent = *rb_node;

		vma = rb_entry(*rb_node, struct vm_area_struct, vm_rb);

		if (vma->vm_end > addr) {
			rb_node = &(*rb_node)->rb_left;		
		} else
			rb_node = &(*rb_node)->rb_right;
	}

	rb_link_node(&vmp->vm_rb, parent, rb_node);
	
	spin_unlock(&mm->page_table_lock);
}
