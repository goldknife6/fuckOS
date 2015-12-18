#include <string.h>

#include <sys/multiboot2.h>


#include <fuckOS/kernel.h>
#include <fuckOS/tty.h>
#include <fuckOS/lapic.h>
#include <fuckOS/trap.h>

#include <mm/slab.h>
#include <mm/mmu.h>

int ap_lock;//AP启动时的锁

int ncpu = 1;

void *mpentry_kstack;

__attribute__ ((aligned(PAGE_SIZE)))
uint8_t percpu_kstacks[CPUNUMS][KERNEL_STKSIZE];




extern void bootmm(void*);
static void gdt_init_percpu();
extern void slab_check();

extern char _BSS_START[],_BSS_END[];



struct seg_descriptor gdt[CPUNUMS + 5] =
{
	// 0x0 - unused (always faults -- for trapping NULL far pointers)
	[0] = set_seg_null,

	// 0x8 - kernel code segment
	[_KERNEL_CS_ >> 3] = set_seg(STA_X | STA_R, 0x0, 0xffffffff, 0),

	// 0x10 - kernel data segment
	[_KERNEL_DS_ >> 3] = set_seg(STA_W, 0x0, 0xffffffff, 0),

	// 0x18 - user code segment
	[_USER_CS_ >> 3] = set_seg(STA_X | STA_R, 0x0, 0xffffffff, 3),

	// 0x20 - user data segment
	[_USER_DS_ >> 3] = set_seg(STA_W, 0x0, 0xffffffff, 3),

	// Per-CPU TSS descriptors (starting from GD_TSS0) are initialized
	// in trap_init_percpu()
	[_TSS0_ >> 3] = set_seg_null
};

struct descriptor_addr gdt_pd = {
	sizeof(gdt) - 1, (unsigned long) gdt
};

void os_entry(void* ginfo,uint32_t gmagic)
{
	//清空BSS段
	memset(_BSS_START,0x0,_BSS_END - _BSS_START);

	console_init(0xB8000);

	gdt_init_percpu();

	//检测内存映射信息，然后计算关于物理页的各种信息 bootmm.c
	//初始化伙伴系统
	bootmm(ginfo);

	//slab_check();

	task_init();

	lapic_init();

	trap_init();

	lapic_startup();
}

void find_next_cpu_stack()
{
	mpentry_kstack = percpu_kstacks[ncpu] + KERNEL_STKSIZE;
}
void mp_main()
{
	lcr3(pgd2p(kpgd));

	gdt_init_percpu();
	
	lapic_init();

	trap_init_percpu();
	
	ap_lock = 0;

	printk("SMP:%d start up ncpu:%d\n",get_cpuid(),ncpu);

	
}
struct node
{
	char a[999];
};
void slab_check()
{
	struct node* n;
	int count = 0;
	while(1){
		n = kmalloc(sizeof(struct node));
		if(!n) break;
		memset(n,0xFF,sizeof(struct node));
		count++;
	}
	printk("count:%d\n",count);
}

static void 
gdt_init_percpu()
{
	lgdt(&gdt_pd);
	asm volatile("movw %%ax,%%gs" :: "a" (_USER_DS_ | 3));
	asm volatile("movw %%ax,%%fs" :: "a" (_USER_DS_ | 3));
	asm volatile("movw %%ax,%%es" :: "a" (_KERNEL_DS_));
	asm volatile("movw %%ax,%%ds" :: "a" (_KERNEL_DS_));
	asm volatile("movw %%ax,%%ss" :: "a" (_KERNEL_DS_));
	asm volatile("ljmp %0,$1f\n 1:\n" :: "i" (_KERNEL_CS_));
	lldt(0);
}

