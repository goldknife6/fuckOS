#include <string.h>

#include <sys/multiboot2.h>
#include <asm/bitops.h>


#include <fuckOS/kernel.h>
#include <fuckOS/tty.h>
#include <fuckOS/lapic.h>
#include <fuckOS/trap.h>
#include <fuckOS/sched.h>
#include <fuckOS/pic_8259A.h>
#include <fuckOS/fs.h>
#include <fuckOS/pci.h>


#include <mm/slab.h>
#include <mm/mmu.h>
#include <mm/pages.h>
#include <mm/mmzone.h>
int ap_lock;//AP启动时的锁

int ncpu = 1;

void *mpentry_kstack;

__attribute__ ((aligned(PAGE_SIZE)))
uint8_t percpu_kstacks[CPUNUMS][KERNEL_STKSIZE];




extern void bootmm(void*);
static void gdt_init_percpu();
extern void slab_check();
extern void fs_init();
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
extern void get_zone_info(void*);
void os_entry(void* ginfo,uint32_t gmagic)
{
	//清空BSS段
	memset(_BSS_START,0x0,_BSS_END - _BSS_START);

	console_init(0xB8000);

	//初始化全局描述符表
	gdt_init_percpu();

	//检测内存映射信息，然后计算关于物理页的各种信息 bootmm.c
	//初始化伙伴系统，slab分配器
	bootmm(ginfo);

	init_8259A();

	lapic_init();

	//中断向量表初始化
	trap_init();

	
	//调度队列初始化
	schedule_init();

	//buffer_init();

	//pci_init();

	ide_init();

	filesystem_init();

	//AP初始化
	//ap_startup();
	TASK_CREATE(testopen, TASK_TYPE_USER);
	//TASK_CREATE(forktree, TASK_TYPE_USER);
	
	schedule();
}

void kthread_create()
{

}

void find_next_cpu_stack()
{
	mpentry_kstack = percpu_kstacks[get_cpuid()] + KERNEL_STKSIZE;
}

void mp_main()
{
	lcr3(pgd2p(kpgd));

	gdt_init_percpu();
	
	lapic_init();

	find_next_cpu_stack();

	__asm __volatile("movl %0,%%esp" : : "r"(mpentry_kstack));

	trap_init_percpu();
	
	schedule_init();

	ap_lock = 0;
	
	printk("SMP:%d start up ncpu:%d\n",get_cpuid(),ncpu);
	TASK_CREATE(testmalloc, TASK_TYPE_USER);
	get_zone_info(&zone_normal);
	schedule();
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
