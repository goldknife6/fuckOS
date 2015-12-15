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
 *								|
 *			CPU0's KERNEL_TEMP 	KERNEL_TEMP	|
 *			Invalid Memory (*)	PAGE_SIZE	|
 *			CPU1's KERNEL_TEMP 	KERNEL_TEMP	|
 *			Invalid Memory (*)	PAGE_SIZE	|
 *								|
 *	KERNEL_STACKBOTT-------------------------------------->0xFEC00000
 *
 *			
 *	KERNEL_NORMAL----------------------------------------->0xF0000000
 *								RW/--
 *	KERNEL_BASE_ADDR/KERN_STACKTOP------------------------>0xC0000000 
 *								PAGE_SIZE
 *	KERN_STACKBOTT/USER_TEMPTOP--------------------------->0xBFFFF000 
 *						     		PTSIZE
 *	USER_STACKTOP/USER_TEMPBOTT--------------------------->0xBF800000			      . 
 *				      .				PTSIZE
 *	USER_STACKBOTT---------------------------------------->0xBF400000
 *
 *
 *
 */
#define	KERNEL_NORMAL		(0xF0000000)

#define KERNEL_BASE_ADDR 	(0xC0000000) 

#define	KERNEL_STKSIZE		(1 << 12)

#define NORMAL_ADDR		(KERNEL_NORMAL - KERNEL_BASE_ADDR)




#endif
