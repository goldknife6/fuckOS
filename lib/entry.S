#include <mm/mmu.h>
#include <mm/layout.h>

.text
.globl _start
_start:
	// See if we were started with arguments on the stack
	cmpl $USER_STACKTOP, %esp
	jne args_exist
	// If not, push dummy argc/argv arguments.
	// This happens when we are loaded by the kernel,
	// because the kernel does not know about passing arguments.
	pushl $0
	pushl $0

args_exist:
	call main
	//call exit
L2:	jmp L2
		

