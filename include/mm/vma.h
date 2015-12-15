#ifndef _VMA_H_
#define _VMA_H_ 
#include <types.h>

#include <minios/list.h>
#include <minios/rbtree.h>


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

	viraddr_t free_area_cache;

	viraddr_t start_code;
	viraddr_t end_code;

	viraddr_t start_data;
	viraddr_t end_data;
	
	viraddr_t start_brk;
	viraddr_t end_brk;

	viraddr_t start_stack;

	atomic_t mmap_count;
	atomic_t mm_count;
	spinlock_t page_table_lock;
};
#endif/*_VMA_H_*/
