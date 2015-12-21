#include <fuckOS/assert.h>
#include <fuckOS/task.h>
#include <fuckOS/trap.h>
#include <syscall.h>
static int32_t syscall(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

static int sys_exit(pid_t);
static pid_t sys_exofork();
static pid_t sys_getpid();
static void sys_cputs(const char *,size_t );
static viraddr_t sys_brk(viraddr_t);

int syscall_handler(struct frame *tf)
{
	struct pushregs * regs = &(tf->tf_regs);
	return syscall(regs->reg_eax, regs->reg_edx, regs->reg_ecx, regs->reg_ebx, regs->reg_edi, regs->reg_esi);
}


static int32_t 
syscall(uint32_t syscallno, uint32_t a1, 
		uint32_t a2, uint32_t a3, 
			uint32_t a4, uint32_t a5)
{

	switch (syscallno) 
	{
	case SYS_CPUTS : { sys_cputs((const char*)a1, a2); return 0; }
	case SYS_EXIT : { sys_exit((pid_t)a1); return 0; }
	case SYS_EXOFORK:{return sys_exofork();}
	case SYS_GETPID:{return sys_getpid();}
	case SYS_BRK:{return sys_brk((viraddr_t)a1);}
	default: return -EINVAL;
	}
}

static int
sys_exit(pid_t pid)
{

	return 0;
}

static pid_t
sys_exofork(void)
{
	return 0;
}

static pid_t 
sys_getpid(void)  
{  
       return curtask->pid;  
}


extern int brk(viraddr_t);
static viraddr_t 
sys_brk(viraddr_t data_seg)  
{  
       return brk(data_seg);  
}

static void 
sys_cputs(const char *s,size_t len)
{
	//if (!user_mem_assert(curtask, s, len, _PAGE_USER))
		printk("%.*s", len, s);
}
