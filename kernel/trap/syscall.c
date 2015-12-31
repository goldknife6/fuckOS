#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <fuckOS/trap.h>
#include <fuckOS/sched.h>

#include <mm/pages.h>

#include <syscall.h>
static int32_t syscall(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

static int sys_exit(pid_t);
static pid_t sys_clone(int ,int (*)(void*));
static pid_t sys_getpid();
static void sys_cputs(const char *,size_t );
static viraddr_t sys_brk(viraddr_t);
static int sys_read(uint32_t,int8_t*,int32_t);
static int sys_open(char * ,int ,int, int);
static int sys_create(char * ,int,int);
static int sys_mkdir(char *filename, int len,int mode);

//	syscall/exit.c
extern int exit(struct task_struct *);

int syscall_handler(struct frame *tf)
{
	struct pushregs * regs = &(tf->tf_regs);
	int i = syscall(regs->reg_eax, regs->reg_edx, regs->reg_ecx, 
			regs->reg_ebx, regs->reg_edi, regs->reg_esi);
	tf->tf_regs.reg_eax = i;
	return 0;
}


static int32_t 
syscall(uint32_t syscallno, uint32_t a1, 
		uint32_t a2, uint32_t a3, 
			uint32_t a4, uint32_t a5)
{

	switch (syscallno) 
	{
	case SYS_CPUTS : { sys_cputs((const char*)a1, a2); return 0; }
	case SYS_EXIT : { sys_exit((pid_t)a1); panic("fuck!\n");return 0; }
	case SYS_EXOFORK:{return sys_clone((int)a1,(int (*)(void *))a2);}
	case SYS_GETPID:{return sys_getpid();}
	case SYS_BRK:{return sys_brk((viraddr_t)a1);}
	case SYS_READ:{return sys_read((uint32_t)a1,(int8_t*)a2,(int32_t)a3);}
	case SYS_OPEN:{return sys_open((char *)a1,(int)a2,(int)a3,(int)a4);}
	case SYS_CREATE:{return sys_create((char *)a1,(int)a2,(int)a3);}
	case SYS_MKDIR:{return sys_mkdir((char *)a1,(int)a2,(int)a3);}
	default: return -EINVAL;
	}
}

int user_mem_check(struct task_struct *task, const void *va, size_t len, int perm)
{
	pte_t* pte;
	perm = perm | _PAGE_PRESENT;
	uint32_t top = ROUNDUP((uint32_t) va + len, PAGE_SIZE);
	uint32_t bottom = ROUNDDOWN((uint32_t)va, PAGE_SIZE);

	for(; bottom < top; bottom += PAGE_SIZE) {

		if(bottom >= KERNEL_BASE_ADDR || 
		  (pte = page_walk(task->task_pgd, 
	          (viraddr_t)bottom, 0)) == NULL ||  
		  (pte_val(*pte) & perm) != perm) {
			return -EFAULT;
			}
		}

	return 0;
}

int 
user_mem_assert(struct task_struct *task, const void *va, size_t len, int perm)
{
	if (user_mem_check(task, va, len, perm | _PAGE_USER) < 0) {
		printk("[%08x] user_mem_check assertion failure\n", task->pid);
		exit(task);
		return -EFAULT;
	}
	return 0;
}


extern int open(char * ,int ,int ,int);

static int 
sys_open(char *filename ,int len, int flags,int mode)
{	
	return open(filename,len, flags,mode);
}




extern int create(char * ,int,int);

static int 
sys_create(char *filename, int len,int mode)
{
	return create(filename,len,mode);
}

extern int mkdir(char * ,int,int);

static int 
sys_mkdir(char *filename, int len,int mode)
{
	return mkdir(filename,len,mode);
}


extern int read(uint32_t,int8_t *,int32_t);
static int sys_read(uint32_t fd,int8_t *buf,int32_t count)
{
	return read(fd,buf,count);
}

static int
sys_exit(pid_t pid)
{
	int retval;
	struct task_struct *task;
	if ((retval = pid2task(pid, &task, true)) < 0)
		return retval;
	retval = exit(task);
	if(!retval) {
		schedule();
	}
	return retval;
}




//	syscall/fork.c
extern pid_t do_fork(int,struct frame);

static pid_t
sys_clone(int flag,int (*fn)(void*))
{
	struct frame fra = curtask->frame;
	if (flag & CLONE_THREAD) {
		fra.tf_eip = (uint32_t)fn;
	}
	return do_fork(flag,fra);
}




static pid_t 
sys_getpid(void)  
{  
       return curtask->pid;  
}



//	syscall/brk.c
extern int brk(viraddr_t);

static viraddr_t 
sys_brk(viraddr_t data_seg)  
{  
       return brk(data_seg);  
}




static void 
sys_cputs(const char *s,size_t len)
{
	if (!user_mem_assert(curtask, s, len, _PAGE_USER))
		printk("%.*s", len, s);
}
