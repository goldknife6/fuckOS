#include <mm/layout.h>
#include <mm/mmu.h>
#include <sys/multiboot2.h>


#define CHECKSUM 	(-(MULTIBOOT2_HEADER_MAGIC + 0 + _header_end - _header_start))

	
	.text
	.globl  start, _start
start: 
_start:
	jmp 	multiboot_entry

	.balign	MULTIBOOT_HEADER_ALIGN
_header_start:
	.long	MULTIBOOT2_HEADER_MAGIC
	.long	MULTIBOOT_ARCHITECTURE_I386
	.long	_header_end - _header_start
	.long	CHECKSUM
	.balign MULTIBOOT_TAG_ALIGN
	.word MULTIBOOT_HEADER_TAG_END
	.word 0	
	.long 8
_header_end:


multiboot_entry:
	cli	
	movl	$RELOC(bootstacktop),%esp
	pushl	%eax
	pushl	%ebx

	call 	set_up_temp_pg
	movl	$(RELOC(temp_pg)),%ecx
	movl	%ecx,%cr3
	movl	%cr4,%ecx

#ifdef CONFIG_PAE
	orl     $CR4_PAE,%ecx	
#else
	orl     $CR4_PSE,%ecx
#endif
	movl	%ecx, %cr4
	movl	%cr0, %ecx

	orl	$(CR0_PG|CR0_WP), %ecx

	movl	%ecx, %cr0
	movl	$redir,%ecx
	jmp	*%ecx

redir:
	lgdt	desc
	
	movw    $_TEMP_DS_, %ax   	# Our data segment selector
	movw    %ax, %ds                # -> DS: Data Segment
	movw    %ax, %es                # -> ES: Extra Segment
	movw    %ax, %fs                # -> FS
	movw    %ax, %gs                # -> GS
	movw    %ax, %ss                # -> SS: Stack Segment
	ljmp	$_TEMP_CS_,$bigjmp

bigjmp:

	movl	$bootstacktop,%esp
	subl	$8,%esp

	call	os_entry
spin:
	hlt
	jmp spin

.data
	.p2align	12		# force page alignment
	.globl		bootstack
bootstack:
	.space		KERNEL_STKSIZE	
	.globl		bootstack   
bootstacktop:
	.p2align 2 
gdt:
	SEG_NULL					# null seg
	SEG(STA_X|STA_R, 0x0, 0xffffffff)		# code seg
	SEG(STA_W, 0x0, 0xffffffff)	       	 	# data seg
	SEG16(STA_W, 0x0, 0xffffffff)	       	 	# data seg
desc:
	.word   0x17                            	# sizeof(realgdt) - 1
	.long   gdt



