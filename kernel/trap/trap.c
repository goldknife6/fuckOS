#include <fuckOS/trap.h>
#include <fuckOS/cpu.h>
#include <fuckOS/kernel.h>
#include <fuckOS/assert.h>
#include <fuckOS/tty.h>
#include <fuckOS/sched.h>

#include <sys/system.h>

#include <mm/mmu.h>
#include <mm/layout.h>

#include <asm/x86.h>

#include <string.h>
struct gate_descriptor idt[256];
struct descriptor_addr idt_pd = {sizeof(idt) - 1, (int) idt};


static void trap_dispatch(struct frame *tf);
extern void page_fault_handler(struct frame *);
extern void breakpoint_handler(struct frame *);
extern int syscall_handler(struct frame *);
extern void timer_handler(struct frame *);
extern void kbd_handler(struct frame *);


static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < sizeof(excnames)/sizeof(excnames[0]))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ0_VECTOR && trapno <= IRQ15_VECTOR)
		return "Hardware Interrupt";
	return "(unknown trap)";
}


void print_regs(struct pushregs *regs)
{
	printk("  edi  0x%08x\n", regs->reg_edi);
	printk("  esi  0x%08x\n", regs->reg_esi);
	printk("  ebp  0x%08x\n", regs->reg_ebp);
	printk("  oesp 0x%08x\n", regs->reg_oesp);
	printk("  ebx  0x%08x\n", regs->reg_ebx);
	printk("  edx  0x%08x\n", regs->reg_edx);
	printk("  ecx  0x%08x\n", regs->reg_ecx);
	printk("  eax  0x%08x\n", regs->reg_eax);
}

void print_frame(struct frame *tf)
{
	STATIC_INIT_SPIN_LOCK(pflock);
	spin_lock(&pflock);
	printk("TRAP frame at %p from CPU %d\n", tf, get_cpuid());
	print_regs(&tf->tf_regs);
	printk("  es   0x----%04x\n", tf->tf_es);
	printk("  ds   0x----%04x\n", tf->tf_ds);
	printk("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf->tf_trapno == T_PGFLT)
		printk("  cr2  0x%08x\n", rcr2());
	printk("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		printk(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		printk("\n");
	printk("  eip  0x%08x\n", tf->tf_eip);
	printk("  cs   0x----%04x\n", tf->tf_cs);
	printk("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		printk("  esp  0x%08x\n", tf->tf_esp);
		printk("  ss   0x----%04x\n", tf->tf_ss);
	}
	spin_unlock(&pflock);
}

void trap_init_percpu()
{
	extern struct seg_descriptor gdt[CPUNUMS + 5];
	extern int ncpu;
	uint32_t cid = get_cpuid();
	struct taskstate *pts = &(thiscpu->cpu_ts);
	pts->ts_ss0 = _KERNEL_DS_;
	pts->ts_esp0 = KERNEL_STACK_TOP - (KERNEL_STKSIZE + PAGE_SIZE) * cid;
	
	gdt[(_TSS0_ >> 3) + cid] = set_seg(STS_T32A, (uint32_t) (pts), sizeof(struct taskstate), 0);
	gdt[(_TSS0_ >> 3) + cid].s = 0;
	
	ltr(_TSS0_ + cid * sizeof(struct seg_descriptor));
	lidt(&idt_pd);
}


void trap_init()
{
	int i;
	extern uint32_t trap_handlers[];
	for(i = 0;i < 255; i++) {
		setgate(idt[i], 0, _KERNEL_CS_, trap_handlers[i], 0);
	}
	setgate(idt[T_SYSCALL], 0, _KERNEL_CS_, trap_handlers[T_SYSCALL], 3);
	trap_init_percpu();
}

static void
trap_dispatch(struct frame *tf)
{
	switch(tf->tf_trapno) {
	case T_PGFLT: page_fault_handler(tf); break;
	case T_BRKPT : breakpoint_handler(tf); break;
	case T_SYSCALL:	tf->tf_regs.reg_eax = syscall_handler(tf); break;
	case IRQ_SPURIOUS: {
		printk("Spurious interrupt on irq 7\n");
		print_frame(tf);
		return;
	}


	case IRQ_TIMER : timer_handler(tf); break;
	case IRQ_KBD : kbd_handler(tf);break;
	case IRQ_SERIAL : print_frame(tf);break;
	case IRQ_IDE : print_frame(tf);break;
	case IRQ_ERROR : print_frame(tf);break;

	default:{	
		 print_frame(tf);
		 if (tf->tf_cs == _KERNEL_CS_) 
			 panic("unhandled trap in kernel");
		 else {	
			panic("unhandled trap in user");
			 return;	
		 }
		 break;
		}
	}
}

void trap(struct frame *tf)
{
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");
	if ((tf->tf_cs & 3) == 3) {
		
		assert(curtask);
		/*
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}*/

		curtask->frame = *tf;
		tf = &curtask->frame;
	} else {
		print_frame(tf);
		panic("kernel!\n");
	}
	//trap_dispatch(tf);
	schedule();
}
