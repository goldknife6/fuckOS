#include <errno.h>
#include <string.h>

#include <mm/mm.h>
#include <mm/slab.h>
#include <mm/pages.h>
#include <mm/layout.h>

#include <fuckOS/task.h>

#include <asm/atomic.h>

static void vm_link(struct mm_struct*, struct vm_area_struct*);
static void vm_link_del(struct mm_struct* , struct vm_area_struct*);
static struct vm_area_struct* find_vma_prev(struct mm_struct*, viraddr_t);
static viraddr_t get_unmapped_vma(struct mm_struct*, viraddr_t, size_t);
static void vma_unmap_page(struct vm_area_struct* vma);


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

	atomic_inc(&mm->mm_count);
	return mm;
}

void free_mm(struct mm_struct* mm)
{
	

	if(!atomic_dec_and_test(&mm->mm_count))
		return;

	while (mm->mmap) {
		delete_vma(mm->mmap);
	}

	kfree(mm);
}

static void vma_unmap_page(struct vm_area_struct* vma)
{
#ifdef CONFIG_DEBUG
	assert(vma);
#endif
	pgd_t *pgd = vma->vm_mm->mm_pgd;
	viraddr_t start;

	for(start = vma->vm_start; start < vma->vm_end ; start += PAGE_SIZE)
		page_remove(pgd, start);
	
}

void delete_vma(struct vm_area_struct* vma)
{
#ifdef CONFIG_DEBUG
	assert(vma);
#endif
	struct vm_area_struct* tmp;
	struct mm_struct* mm = vma->vm_mm;
	spin_lock(&mm->page_table_lock);

	vma_unmap_page(vma);

	vm_link_del(vma->vm_mm , vma);

	kfree(vma);

	spin_unlock(&mm->page_table_lock);
}

static void 
vma_debug(struct vm_area_struct* vma)
{
	assert(vma);
	printk("vma:%08x vm_start:0x%08x vm_end:0x%08x\n",vma,vma->vm_start,vma->vm_end);
}

struct vm_area_struct* 
create_vma(struct mm_struct* mm, viraddr_t addr, 
			size_t len, uint32_t flags)
{

#ifdef CONFIG_DEBUG
	assert(mm);
#endif
	

	if(!ROUNDDOWN(addr,PAGE_SIZE))  
		return NULL;

	struct vm_area_struct* vma = NULL;
	
	spin_lock(&mm->page_table_lock);

	addr = get_unmapped_vma(mm, addr, len);
	
	if(!addr)  
		goto unlock;

	vma =  kmalloc(sizeof(*vma));

	if(!vma) 
		goto unlock;
	
	vma->vm_mm = mm;
	vma->vm_start = ROUNDDOWN(addr,PAGE_SIZE);
	vma->vm_end = vma->vm_start + ROUNDUP(len,PAGE_SIZE);
	vma->vm_flags = flags;
	vma->vm_next = NULL;
	vm_link(mm, vma);
unlock:
	spin_unlock(&mm->page_table_lock);

	return vma;
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

static struct vm_area_struct*
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

static viraddr_t 
get_unmapped_vma(struct mm_struct* mm, viraddr_t addr, size_t len)
{
	struct vm_area_struct* vma = NULL;

	if(len > KERNEL_BASE_ADDR)
		return 0;

	if (addr) {
		vma = find_vma(mm, addr);
		if (KERNEL_BASE_ADDR  >= addr + len &&
		    (!vma || addr + len <= vma->vm_start))
			return addr;
		else
			return 0;
	} else {
		viraddr_t start_addr = addr = mm->free_area_cache;

		for (vma = find_vma(mm, addr); ; vma = vma->vm_next) {

			if (addr + len > KERNEL_BASE_ADDR) {
				if (start_addr == 0x40000000)
					return 0;
				start_addr = addr = 0x40000000;

				vma = find_vma(mm, addr);
			}

			if (!vma || addr + len <= vma->vm_start) {
				mm->free_area_cache = addr + len;
				return addr;
			}
			addr = vma->vm_end;
		}
	}
	return addr;
}


static void 
vm_link(struct mm_struct* mm, struct vm_area_struct* vmp)
{
	viraddr_t addr = vmp->vm_end;
	struct rb_node** rb_node = &mm->mm_rb.rb_node;
	struct rb_node* parent = NULL;
	struct vm_area_struct* vma;
	

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
	
	mm->mmap_count++;
}

static void 
vm_link_del(struct mm_struct* mm, struct vm_area_struct* vmp)
{

#ifdef CONFIG_DEBUG
	assert(mm && vmp);
#endif

	viraddr_t addr = vmp->vm_end;
	struct rb_node** rb_node = &mm->mm_rb.rb_node;
	struct vm_area_struct* vma;
	struct vm_area_struct* tmp;

	vma = find_vma(mm, vmp->vm_start);

	if (vma != vmp) 
		return;

	tmp = find_vma_prev(mm, vmp->vm_start);

	rbtree_delete(&mm->mm_rb,&vmp->vm_rb);

	mm->mmap_count--;

	if (!tmp)
		mm->mmap = vmp->vm_next;
	else
		tmp->vm_next = vma->vm_next;

	vmp->vm_next = NULL;
}
