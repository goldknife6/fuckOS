#include <mm/mmu.h>
#include <mm/layout.h>
#include <mm/pages.h>

#include <sys/system.h>


#include <asm/x86.h>

#define RELOC(x)	((x) - KERNEL_BASE_ADDR)

#ifdef CONFIG_PAE
__attribute__((__aligned__(PAGE_SIZE))) 
physaddr_t  temp_pg8m[PTRS_PER_PMD] = 
{
	[0] = 0x000000 | _PAGE_RW | _PAGE_PRESENT | _PAGE_PSE,
	[1] = 0x200000 | _PAGE_RW | _PAGE_PRESENT | _PAGE_PSE,
	[2] = 0x400000 | _PAGE_RW | _PAGE_PRESENT | _PAGE_PSE,
	[3] = 0x800000 | _PAGE_RW | _PAGE_PRESENT | _PAGE_PSE
};

__attribute__((__aligned__(PAGE_SIZE))) 
physaddr_t  temp_pg[PTRS_PER_PGD] = 
{
	[0] = 1,
	[1] = 0,
	[2] = 0,
	[3] = 1
};
#else
__attribute__((__aligned__(PAGE_SIZE))) 
physaddr_t  temp_pg[PTRS_PER_PGD] = 
{
	[0] = 0x0 |_PAGE_RW |_PAGE_PRESENT | _PAGE_PSE,
	[1] = 0x1 |_PAGE_RW |_PAGE_PRESENT | _PAGE_PSE,
	[KERNEL_BASE_ADDR >> PGDIR_SHIFT] 	= 0x0 |_PAGE_RW |_PAGE_PRESENT | _PAGE_PSE,
	[(KERNEL_BASE_ADDR >> PGDIR_SHIFT) + 1] = 0x1 |_PAGE_RW |_PAGE_PRESENT | _PAGE_PSE,
};
#endif/*!CONFIG_PAE*/

struct seg_descriptor temp_gdt[6] = 
{
	[0] = set_seg_null,
	[1] = set_seg(STA_X|STA_R, 0x0, 0xFFFFFFFF, 0),
	[2] = set_seg(STA_W, 0x0, 0xFFFFFFFF, 0),
	[3] = set_seg_real_mode(STA_X|STA_R, 0x0, 0xFFFF, 0),
	[4] = set_seg_real_mode(STA_W, 0x0, 0xFFFF, 0),
	[5] = set_seg(STA_W, 0xc0100000, 0xFFFFFFFF, 0)
};

struct descriptor_addr sda = 
{
	.lim = 1,
	.base = 1
};

struct descriptor_addr ridt = 
{
	.lim = 1,
	.base = 1
};


void set_up_temp_pg()
{
#ifdef CONFIG_PAE
	physaddr_t* redir = (physaddr_t* )RELOC((viraddr_t)temp_pg);
	redir[0] =  RELOC((viraddr_t)temp_pg8m) | _PAGE_PRESENT;
	redir[3] =  RELOC((viraddr_t)temp_pg8m) | _PAGE_PRESENT;
#endif
}

void set_up_temp_gdt()
{
	sda.lim = sizeof(temp_gdt)*3 - 1;
	sda.base = RELOC((uint32_t)temp_gdt);
	lgdt(&sda);
}
