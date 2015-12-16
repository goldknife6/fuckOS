#include <string.h>

#include <fuckOS/kernel.h>
#include <fuckOS/tty.h>

#include <sys/multiboot2.h>

extern void bootmm(void*);

extern char _BSS_START[],_BSS_END[];

int i;
void os_entry(void* ginfo,uint32_t gmagic)
{
	
	memset(_BSS_START,0x0,_BSS_END - _BSS_START);

	console_init(0xB8000);


	if ( MULTIBOOT2_BOOTLOADER_MAGIC != gmagic)
		printk("GrubMultiInfo:%x GrubMagic:%x\n",ginfo,gmagic);

	//检测内存映射信息，然后计算关于物理页的各种信息 bootmm.c
	bootmm(ginfo);



}
