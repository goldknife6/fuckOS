#include <fuckOS/trap.h>
#include <fuckOS/cpu.h>

#include <sys/system.h>

#include <mm/mmu.h>
#include <mm/layout.h>

#include <asm/x86.h>
struct gate_descriptor idt[256];
struct descriptor_addr idt_pd = {sizeof(idt) - 1, (int) idt};

void trap_init_percpu()
{
	extern struct seg_descriptor gdt[CPUNUMS + 5];
	extern int ncpu;
	uint32_t cid = get_cpuid();
	struct taskstate *pts = &(thiscpu->cpu_ts);
	pts->ts_ss0 = _KERNEL_DS_;

	gdt[(_TSS0_ >> 3) + cid] = set_seg(STS_T32A, (uint32_t) (pts), sizeof(struct taskstate), 0);
	gdt[(_TSS0_ >> 3) + cid].s = 0;
	
	ltr(_TSS0_ + cid * sizeof(struct seg_descriptor));
	lidt(&idt_pd);
}

void trap_init()
{
	int i;
	extern uint32_t trap_handlers[];

	for(i = 0;i < 256; i++) {
		setgate(idt[i], 0, _KERNEL_CS_, trap_handlers[i], 0);
	}
	
	setgate(idt[T_SYSCALL], 0, _KERNEL_CS_, trap_handlers[T_SYSCALL], 3);

	trap_init_percpu();
}
void trap(struct frame *tf)
{
	
}
