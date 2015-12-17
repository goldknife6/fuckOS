#ifndef _MINIOS_TASK_H
#define _MINIOS_TASK_H

#include <types.h>

#include <mm/mm.h>

#include <sys/system.h>
#include <fuckOS/list.h>


/* max pid, equal to 2^15=32768 */
#define PID_MAX_DEFAULT 0x8000


#define BITS_PER_BYTE 8
//4k*8 32768
#define BITS_PER_PAGE (PAGE_SIZE * BITS_PER_BYTE)
//7fff 
//0111 1111 1111 1111
#define BITS_PER_PAGE_MASK (BITS_PER_PAGE - 1)

enum task_status{
	TASK_RUNNING,
	TASK_INTERRUPTIBLE,
	TASK_UNINTERRUPTIBLE,
	TASK_DYING
};

enum task_type {
	TASK_TYPE_USER = 0,
	TASK_TYPE_FS,		// File system server
	TASK_TYPE_NS,		// Network server
};

enum clone_type {
	CLONE_KTHREAD = 1<<0,
	CLONE_VM = 1<<1,		
};


enum {
	_NEED_RESCHED = 1 << 0
};

struct thread_info
{
	uint32_t esp;
	uint32_t eip;
	struct frame *trapframe;
};

struct task_struct
{
	struct thread_info thread_info;
	struct task_struct* child;
	pid_t pid;			
	pid_t ppid;
	int32_t  task_cpunum;
	int32_t  task_status;
	int32_t  task_type;
	struct mm_struct* mm;
	pgd_t* task_pgd;


	int32_t timeslice;
	int32_t prio;
	int32_t static_prio;
	int32_t flags;
	int32_t runnum;
	struct list_head list;
	struct list_head wait_list;

	//struct rb_node sched_entry;
};


struct pidmap 
{
    uint32_t nr_free;
    uint8_t page[4096];
};

#define TASK_PASTE3(x, y, z) x ## y ## z

#define TASK_CREATE(x, type,ppid,store)						\
	do {									\
		extern uint8_t TASK_PASTE3(_binary_, x, _start)[];		\
		task_create(TASK_PASTE3(_binary_, x, _start),			\
			   type,ppid,store);					\
	} while (0)

#define curtask	 	(thiscpu->cpu_task)


extern struct task_struct* task_pidmap[];


#endif/*_MINIOS_TASK_H*/
