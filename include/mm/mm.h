#ifndef _MM_H_
#define _MM_H_ 

#ifdef CONFIG_PAE
# include "pgtable-3level.h"
#else
# include "pgtable-2level.h"
#endif

#include <types.h>
#include <errno.h>

#include <fuckOS/list.h>
#include <fuckOS/rbtree.h>

#include <asm/atomic.h>

enum {
	VM_READ = 1 << 0,
	VM_WRITE  = 1 << 1,
	VM_EXEC  = 1 << 2,
	VM_SHARED  = 1 << 3,
	VM_FILE  = 1 << 4,
	VM_IPC  = 1 << 5
};


struct vm_area_struct
{
	struct mm_struct* vm_mm;
	struct vm_area_struct* vm_next;
	struct rb_node vm_rb;

	viraddr_t vm_start;
	viraddr_t vm_end;
	uint32_t vm_flags;
};

struct mm_struct
{
	struct vm_area_struct* mmap;
	struct rb_root mm_rb;
	pgd_t* mm_pgd;

	viraddr_t free_area_cache;

	viraddr_t start_code;
	viraddr_t end_code;

	viraddr_t start_data;
	viraddr_t end_data;
	
	viraddr_t start_brk;
	viraddr_t end_brk;

	viraddr_t start_stack;

	int mmap_count;
	atomic_t mm_count;
	spinlock_t page_table_lock;
};
extern struct mm_struct* alloc_mm();
extern void free_mm(struct mm_struct*);
extern struct vm_area_struct* create_vma(struct mm_struct*,viraddr_t,size_t,uint32_t);
extern void delete_vma(struct vm_area_struct*);
extern struct vm_area_struct* find_vma(struct mm_struct*, viraddr_t);
#endif/*_MM_H_*/
