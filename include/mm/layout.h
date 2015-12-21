#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_ 



/*
 * Virtual memory map:                                		Permissions
 *                                                   		kernel/user
 *
 *	4 Gig ------------------------------------------------>0xFFFFFFFF
 *								PTSIZE
 *	KERNEL_LIMIT/KERNEL_MMIO_LIMIT------------------------>0xFFC00000
 *								PTSIZE
 *	KERNEL_MMIO------------------------------------------->0xFF800000
 *				Gap.				PTSIZE
 *	------------------------------------------------------>0xFF400000
 * 								PTSIZE
 *	KERNEL_VIDEO & KERNEL_TEMP---------------------------->0xFF000000
 *		
 *			  
 *	KERNEL_STACK_TOP-------------------------------------->0xF0800000
 *		
 *			CPU  Kernel Stack PAGE_SIZE
 *			
 *			 Invalid Memory (*) PAGE_SIZE
 *
 *			
 *	KERNEL_NORMAL/KERNEL_STACK---------------------------->0xF0000000
 *								RW/--
 *	KERNEL_BASE_ADDR-------------------------------------->0xC0000000 
 *								
 *	USER_STACKTOP/USER_TEMPBOTT--------------------------->0xBF800000			      . 
 *				      .				PTSIZE
 *	USER_STACKBOTT---------------------------------------->0xBF400000
 *
 *
 *	USER_UNNAME_ZONE-------------------------------------->0x40000000
 *
 *
 *	USER_BRK_ZONE----------------------------------------->0x10000000
 *
 *
 *
 */
#define	KERNEL_NORMAL		(0xF0000000)

#define KERNEL_BASE_ADDR 	(0xC0000000) 

#define	KERNEL_STKSIZE		(1 << 12)

#define KERNEL_VIDEO		(0xFF000000)

#define KERNEL_MMIO		(0xFF800000)

#define KERNEL_MMIO_LIMIT	(0xFFC00000)

#define KERNEL_STACK		(0xF0000000)

#define KERNEL_STACK_TOP	(0xF0800000)

#define NORMAL_ADDR		(KERNEL_NORMAL - KERNEL_BASE_ADDR)


#define	USER_UNNAME_ZONE	(0x40000000)
#define USER_STACKTOP		(0xBF800000)
#define USER_STACKBOTT		(0xBF400000)
#define USER_STACK_SIZE		(PT_SIZE)


#define	PT_SIZE			(PAGE_SIZE << 10)

#define MPENTRY_PADDR		(0x7000)

#endif
