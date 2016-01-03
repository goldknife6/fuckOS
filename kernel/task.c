#include <fuckOS/task.h>
#include <fuckOS/sched.h>
#include <fuckOS/pidmap.h>
#include <fuckOS/fs.h>

#include <sys/elf.h>

#include <mm/pages.h>
#include <mm/mmu.h>

#include <errno.h>
#include <string.h>


static int load_icode(struct task_struct *, uint8_t *);
static int region_alloc(struct task_struct *, viraddr_t, size_t ,int );
static int task_alloc(struct task_struct **, pid_t);
struct task_struct* task_pidmap[PID_MAX_DEFAULT];
static int alloc_files_struct(struct task_struct *);



static int
load_icode(struct task_struct *task, uint8_t *binary)
{
	assert(task);
	assert(binary);
	int r;
	struct vm_area_struct* vma = NULL;
	ElfHeader* eh = (ElfHeader*)binary;

	if(eh->e_magic != ELF_MAGIC) {
		return -ENOEXEC;
	}

	ProgHeader* ph = (ProgHeader*)((uint8_t *)eh + eh->e_phoff);

	ProgHeader* eph = ph + eh->e_phnum;

	lcr3(pgd2p(task->mm->mm_pgd));


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
		
		vma = create_vma(task->mm, ph->p_va, ph->p_memsz, VM_READ | VM_WRITE);
	
		if (!vma)
			return -ENOMEM;

		r = region_alloc(task, ph->p_va, ph->p_memsz,
				_PAGE_PRESENT | _PAGE_RW |_PAGE_USER);
		if (r < 0) 
			return r;
		memcpy((void *)ph->p_va, (void *)(binary + ph->p_offset), ph->p_filesz);
		if ( ph->p_filesz > ph->p_memsz )  
			return -ENOEXEC;
		else 
			memset((char *)ph->p_va + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}
	
	//用户栈区域
	vma = create_vma(task->mm, USER_STACKBOTT, USER_STACK_SIZE, VM_READ | VM_WRITE);
	if (!vma) {
		panic("vma\n");	
		return -ENOMEM;
	}
	
	//用户堆区域
	task->mm->start_brk = (task->mm->end_data ? task->mm->end_data : task->mm->end_code);
	task->mm->start_brk = ROUNDUP(task->mm->start_brk,PAGE_SIZE);
	task->mm->end_brk = task->mm->start_brk + PAGE_SIZE;
	vma = create_vma(task->mm, task->mm->start_brk, PAGE_SIZE , VM_READ | VM_WRITE);
	if (!vma) {
		panic("vma\n");			
		return -ENOMEM;
	}

	

	task->frame.tf_eip = eh->e_entry;

	lcr3(pgd2p(kpgd));
	return 0;
}

static int
region_alloc(struct task_struct *task, viraddr_t va, size_t len,int perm)
{
	assert(task);
	assert(va);
	assert(len > 0);
	int r;
	struct page* page;
	viraddr_t down = ROUNDDOWN(va,PAGE_SIZE);
	viraddr_t up = ROUNDUP(va + len,PAGE_SIZE);	
	for (;down < up;down += PAGE_SIZE) {
		page = page_alloc(_GFP_ZERO);
		
		if(page == NULL) 
			return -ENOMEM;

		r = page_insert(task->mm->mm_pgd, page, down, perm);
		if(r < 0) 
			return r;
	}
	return 0;
}

int task_set_vm(struct task_struct *task)
{
	struct page *page = NULL;
	pgd_t *pgd;
	task->mm = alloc_mm();

	if (!task->mm)
		return -ENOMEM;
	
	page = page_alloc(_GFP_ZERO);
	if (!page) {
		kfree(task->mm);
		return -ENOMEM;
	}
	pgd = (pgd_t *)page2virt(page);
	task->task_pgd = task->mm->mm_pgd = pgd;
	memmove(pgd,kpgd,PAGE_SIZE);
	return 0;
}

static int alloc_files_struct(struct task_struct *task)
{
	assert(task);
	task->files = kmalloc(sizeof(struct files_struct));
	if (!task->files)
		return -ENOMEM;

	task->fs = kmalloc(sizeof(struct fs_struct));
	if (!task->fs)
		return -ENOMEM;
	memset(task->files,0,sizeof(struct files_struct));
	memset(task->fs,0,sizeof(struct fs_struct));

	task->fs->pwd = root_dentry;
	task->fs->root = root_dentry;
	task->fs->mnt_pwd = root_vfsmnt;
	task->fs->mnt_root = root_vfsmnt;

	return 0;
}

static int
task_alloc(struct task_struct **newenv_store, pid_t parent_id)
{
	struct task_struct *task;
	int reval = 0;
	task = kmalloc(sizeof(struct task_struct));
	if (!task) {
		assert(0);
		return -ENOMEM;
	}
	reval = alloc_files_struct(task);
	assert(reval >= 0);

	task->fs->pwd = root_dentry;
	task->fs->root = root_dentry;
	task->fs->mnt_pwd = root_vfsmnt;
	task->fs->mnt_root = root_vfsmnt;

	reval = task_set_vm(task);
	if (reval < 0) {
		return reval;
	}

	task->pid = alloc_pidmap();
	if (task->pid < 0) {
		return -EMPROC;
	}
	// Set the basic status variables.
	task->ppid = parent_id;
	task->task_type = TASK_TYPE_USER;
	task->task_status = TASK_RUNNING;
	task->timeslice = 10;

	memset(&task->frame, 0, sizeof(struct frame));


	task->frame.tf_ds = _USER_DS_ | 3;
	task->frame.tf_es = _USER_DS_ | 3;
	task->frame.tf_ss = _USER_DS_ | 3;
	task->frame.tf_esp = USER_STACKTOP;
	task->frame.tf_cs = _USER_CS_ | 3;
	
	task->frame.tf_eflags = FL_IF;

	if(newenv_store)
		*newenv_store  = task;
	return 0;
}

int
task_create(uint8_t *binary, enum task_type type)
{
	int reval;
	struct task_struct *task;
	reval = task_alloc(&task, 0);
	if (reval < 0) {
		assert(0);		
		return reval;
	}

	reval = load_icode(task, binary);
	if (reval < 0) {
		assert(0);
		return reval;
	}
	task->task_type = type;
	task_pidmap[task->pid] = task;
	schedule_add_task(task);
	return 0;
}

void
task_pop_tf(struct frame *tf)
{
	// Record the CPU we are running on for user-space debugging
	
	__asm __volatile("movl %0,%%esp\n"
		"\tpopal\n"
		"\tpopl %%es\n"
		"\tpopl %%ds\n"
		"\taddl $0x8,%%esp\n" /* skip tf_trapno and tf_errcode */
		"\tiret"
		: : "g" (tf) : "memory");
	panic("iret failed");  /* mostly to placate the compiler */
}

void print_frame(struct frame *tf);

int
task_run(struct task_struct *task)
{
	curtask = task;
	
	lcr3(pgd2p(curtask->mm->mm_pgd));
	task_pop_tf(&task->frame);
	return 0;
}


int 
pid2task(pid_t pid, struct task_struct **taskstore, bool checkperm)
{
	int r;
	assert(pid>=0);
	assert(pid<PID_MAX_DEFAULT);
	
	struct task_struct *task;

	if (pid == 0) {
		*taskstore = curtask;
		return 0;
	}

	task = task_pidmap[pid];
	
	if(!task || task->pid != pid) {
		return -EBADP;
	}

	if (checkperm && task != curtask && task->ppid != curtask->pid) {
		*taskstore = 0;
		return -EBADP;
	}

	*taskstore = task;

	return 0;
}

