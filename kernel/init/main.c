#include <string.h>

#include <sys/multiboot2.h>

#include <fuckOS/kernel.h>
#include <fuckOS/tty.h>

#include <mm/slab.h>

extern void bootmm(void*);

extern char _BSS_START[],_BSS_END[];

void slab_check();

int ap_lock;//AP启动时的锁

int ncpu = 1;

void *mpentry_kstack;

__attribute__ ((aligned(PAGE_SIZE)))
uint8_t percpu_kstacks[CPUNUMS][KERNEL_STKSIZE];

void os_entry(void* ginfo,uint32_t gmagic)
{
	//清空BSS段
	memset(_BSS_START,0x0,_BSS_END - _BSS_START);

	console_init(0xB8000);

	//检测内存映射信息，然后计算关于物理页的各种信息 bootmm.c
	//初始化伙伴系统
	bootmm(ginfo);


	slab_check();
}
void mp_main()
{
	
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
