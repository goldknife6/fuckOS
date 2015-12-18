#include <fuckOS/task.h>


#include <mm/slab.h>
#include <mm/pages.h>

#include <sys/elf.h>

#include <string.h>
struct task_struct *init_task;
struct pidmap pidmap = { PID_MAX_DEFAULT, {0}};
struct task_struct* task_pidmap[PID_MAX_DEFAULT];
static pid_t last_pid = -1;


struct task_struct *curtasks[CPUNUMS];
void task_init()
{
	init_task = alloc_task();
	assert(init_task);

	init_task->mm = kmalloc(sizeof(struct mm_struct));
	memset(init_task->mm,0x0,sizeof(struct mm_struct));
	assert(init_task->mm);
	init_task->mm->mm_pgd = kpgd;
}

static int 
test_and_set_bit(int offset, void *addr)
{
    uint32_t mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));
    uint32_t *p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));
    uint32_t old = *p;

    *p = old | mask; 

    return (old & mask) != 0;
}

static void 
clear_bit(int offset, void *addr)
{
    uint32_t mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));//取offset的后31位数据,并左移
    uint32_t *p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));//+优先级高于>>
    uint32_t old = *p;
    *p = old & ~mask;
}

static int 
find_next_zero_bit(void *addr, int size, int offset)
{
    uint32_t *p;
    uint32_t mask;

    while (offset < size)
    {
        p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));
        mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));

        if ((~(*p) & mask))
        {
            break;
        }
        ++offset;
    }

    return offset;
}

STATIC_INIT_SPIN_LOCK(pidlock);
int 
alloc_pidmap()
{
	pid_t pid = last_pid + 1;
	int offset = pid & BITS_PER_PAGE_MASK;//把offset的最高为变为0，其他的不变
	spin_lock(&pidlock);
	if (!pidmap.nr_free)
	{
		spin_unlock(&pidlock);
		return -1;
	}

	offset = find_next_zero_bit(&pidmap.page, BITS_PER_PAGE, offset);

	if (BITS_PER_PAGE != offset && !test_and_set_bit(offset, &pidmap.page))
	{
		--pidmap.nr_free;
		last_pid = offset;
		spin_unlock(&pidlock);
		return offset;
	}
	spin_unlock(&pidlock);
   	return -1;
}

void 
free_pidmap(pid_t pid)
{
    	int offset = pid & BITS_PER_PAGE_MASK;
	spin_lock(&pidlock);
   	pidmap.nr_free++;
   	clear_bit(offset, &pidmap.page);
	spin_unlock(&pidlock); 

}

struct task_struct *alloc_task()
{
	struct task_struct *task;
	task = kmalloc(sizeof(struct task_struct));

	if(!task)
		return NULL;
	memset(task,0,sizeof(struct task_struct));
	task->pid = alloc_pidmap();
	if (task->pid < 0 ) {
		kfree(task);
		task = NULL;
	}
	return task;
}


int task_create(void *binary,int type,pid_t ppid,
			struct task_struct **sttask)
{
	struct task_struct *task = NULL;
	if (sttask)
		*sttask = task;

	task = alloc_task();
	if (!task)
		return -ENOMEM;
	return 0;
}


static int 
region_alloc(struct task_struct *task, 
		viraddr_t va, size_t len,int perm)
{
	assert(task);
	assert(va);
	assert(len > 0);
	int r;
	viraddr_t down = ROUNDDOWN(va,PAGE_SIZE);
	viraddr_t up = ROUNDUP(va + len,PAGE_SIZE);	
	for (;down < up;down += PAGE_SIZE) {
		struct page* page = page_alloc(_GFP_NONE);
		
		if(page == NULL) 
			return -ENOMEM;

		r = page_insert(task->mm, page, down, perm);
		if(r < 0) 
			return r;
	}
	return 0;
}

static int  
task_load_code(struct task_struct *task,uint8_t *binary)
{
	assert(task);
	assert(binary);
	int r;
	struct frame *tp;
	ElfHeader* eh = (ElfHeader*)binary;

	if(eh->e_magic != ELF_MAGIC) {
		return -ENOEXEC;
	}

	ProgHeader* ph = (ProgHeader*)((uint8_t *)eh + eh->e_phoff);

	ProgHeader* eph = ph + eh->e_phnum;

	lcr3(pgd2p(task->task_pgd));


	for(ph;ph < eph;ph++) {
		if (ph->p_type != ELF_PROG_LOAD)	
			continue;

		if ((ph->p_flags & ELF_PROG_FLAG_EXEC) && 
		   !(ph->p_flags & ELF_PROG_FLAG_WRITE)) {	
			task->mm->start_code = ph->p_va;
			task->mm->end_code = ph->p_va + ph->p_memsz;
		} else {
			task->mm->start_data = ph->p_va;
			task->mm->end_data = ph->p_va + ph->p_memsz;
		}

		r = region_alloc(task, ph->p_va, ph->p_memsz,_PAGE_PRESENT | _PAGE_RW |_PAGE_USER);

		if (r < 0) 
			return r;


		memcpy((void *)ph->p_va, (void *)(binary + ph->p_offset), ph->p_filesz);
		
		if ( ph->p_filesz > ph->p_memsz )  
			return -ENOEXEC;
		else 
			memset((char *)ph->p_va + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}


	//tp->tf_eip = eh->e_entry;

	lcr3(pgd2p(kpgd));

	return 0;
}
