#include <mm/mmu.h>
#include <mm/layout.h>


#define MPBOOTPHYS(s) ((s) - mpentry_start + MPENTRY_PADDR)

.set PROT_MODE_CSEG, 0x8	# kernel code segment selector
.set PROT_MODE_DSEG, 0x10	# kernel data segment selector

.code16           
	.globl mpentry_start
mpentry_start:
	cli            
	xorw    %ax, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss

	lgdt    MPBOOTPHYS(gdtdesc)

	movl    %cr0, %eax
	orl     $CR0_PE, %eax
	movl    %eax, %cr0


	ljmpl   $(PROT_MODE_CSEG), $(MPBOOTPHYS(start32))


	.code32
start32:

	movw    $(PROT_MODE_DSEG), %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	movw    %ax, %fs
	movw    %ax, %gs

	movl	$(RELOC(temp_pg)),%eax
	movl    %eax, %cr3

	movl	%cr4,%ecx

#ifdef CONFIG_PAE
	orl     $CR4_PAE,%ecx	
#else
	orl     $CR4_PSE,%ecx
#endif
	movl	%ecx, %cr4

	movl    %cr0, %eax
	orl     $(CR0_PG|CR0_WP), %eax
	movl    %eax, %cr0


tset_ap_lock:
	lock btsl $0,ap_lock
	pause
	jc 	tset_ap_lock

	addl	$1,ncpu
	movl	$mpstacktop,%esp
	movl    $0x0, %ebp       # nuke frame pointer
	
	movl    $find_next_cpu_stack, %eax
	call    *%eax
	movl    mpentry_kstack,%esp

	movl    $mp_main, %eax
	call    *%eax
spin:
	hlt
	jmp     spin


	.p2align 2				# force 4 byte alignment
gdt:
	SEG_NULL				# null seg
	SEG(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
	SEG(STA_W, 0x0, 0xffffffff)		# data seg
gdtdesc:
	.word   0x17				# sizeof(gdt) - 1
	.long   MPBOOTPHYS(gdt)			# address gdt
	.globl mpentry_end
mpentry_end:



	.data
mpstack:
	.space		KERNEL_STKSIZE
mpstacktop:
