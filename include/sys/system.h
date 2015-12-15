#ifndef _MINIOS_SYSTEM_H
#define _MINIOS_SYSTEM_H
#include <types.h>

struct seg_descriptor {
	unsigned lim_15_0 : 16;  // Low bits of segment limit
	unsigned base_15_0 : 16; // Low bits of segment base address
	unsigned base_23_16 : 8; // Middle bits of segment base address
	unsigned type : 4;       // Segment type (see STS_ constants)
	unsigned s : 1;          // 0 = system, 1 = application
	unsigned dpl : 2;        // Descriptor Privilege Level
	unsigned p : 1;          // Present
	unsigned lim_19_16 : 4;  // High bits of segment limit
	unsigned avl : 1;        // Unused (available for software use)
	unsigned rsv1 : 1;       // Reserved
	unsigned db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
	unsigned g : 1;          // Granularity: limit scaled by 4K when set
	unsigned base_31_24 : 8; // High bits of segment base address
} __attribute__ ((packed));

struct gate_descriptor {
	unsigned gd_off_15_0 : 16;   // low 16 bits of offset in segment
	unsigned gd_sel : 16;        // segment selector
	unsigned gd_args : 5;        // # args, 0 for interrupt/trap gates
	unsigned gd_rsv1 : 3;        // reserved(should be zero I guess)
	unsigned gd_type : 4;        // type(STS_{TG,IG32,TG32})
	unsigned gd_s : 1;           // must be 0 (system)
	unsigned gd_dpl : 2;         // descriptor(meaning new) privilege level
	unsigned gd_p : 1;           // Present
	unsigned gd_off_31_16 : 16;  // high bits of offset in segment
};


struct descriptor_addr {
	uint16_t lim;		// Limit
	uint32_t base;		// Base address
} __attribute__ ((packed));

struct pushregs {
	/* registers as pushed by pusha */
	uint32_t reg_edi;
	uint32_t reg_esi;
	uint32_t reg_ebp;
	uint32_t reg_oesp;		/* Useless */
	uint32_t reg_ebx;
	uint32_t reg_edx;
	uint32_t reg_ecx;
	uint32_t reg_eax;
} __attribute__((packed));

struct frame {
	struct pushregs tf_regs;
	uint16_t tf_es;
	uint16_t tf_padding1;
	uint16_t tf_ds;
	uint16_t tf_padding2;
	uint32_t tf_trapno;
	/* below here defined by x86 hardware */
	uint32_t tf_err;
	uint32_t tf_eip;
	uint16_t tf_cs;
	uint16_t tf_padding3;
	uint32_t tf_eflags;
	/* below here only when crossing rings, such as from user to kernel */
	uint32_t tf_esp;
	uint16_t tf_ss;
	uint16_t tf_padding4;
} __attribute__((packed));


struct taskstate {
	uint32_t ts_link;	// Old ts selector
	uint32_t ts_esp0;	// Stack pointers and segment selectors
	uint16_t ts_ss0;	//   after an increase in privilege level
	uint16_t ts_padding1;
	uint32_t ts_esp1;
	uint16_t ts_ss1;
	uint16_t ts_padding2;
	uint32_t ts_esp2;
	uint16_t ts_ss2;
	uint16_t ts_padding3;
	uint32_t ts_cr3;	// Page directory base
	uint32_t ts_eip;	// Saved state from last task switch
	uint32_t ts_eflags;
	uint32_t ts_eax;	// More saved state (registers)
	uint32_t ts_ecx;
	uint32_t ts_edx;
	uint32_t ts_ebx;
	uint32_t ts_esp;
	uint32_t ts_ebp;
	uint32_t ts_esi;
	uint32_t ts_edi;
	uint16_t ts_es;		// Even more saved state (segment selectors)
	uint16_t ts_padding4;
	uint16_t ts_cs;
	uint16_t ts_padding5;
	uint16_t ts_ss;
	uint16_t ts_padding6;
	uint16_t ts_ds;
	uint16_t ts_padding7;
	uint16_t ts_fs;
	uint16_t ts_padding8;
	uint16_t ts_gs;
	uint16_t ts_padding9;
	uint16_t ts_ldt;
	uint16_t ts_padding10;
	uint16_t ts_t;		// Trap on task switch
	uint16_t ts_iomb;	// I/O map base address
};



#define set_seg(type, base, limt, dpl) (struct seg_descriptor)\
	{(limt) & 0xFFFF,(base) & 0xFFFF,((base) >> 16) & 0xFF,type, 1, dpl , 1, ((limt) >> 16) & 0xF, 0, 0,1,1,((base)>>24) & 0xFF}

#define set_seg_real_mode(type, base, limt, dpl) (struct seg_descriptor)\
	{(limt) & 0xFFFF,(base) & 0xFFFF,((base) >> 16) & 0xFF,type, 1, dpl , 1, ((limt) >> 16) & 0xF, 0, 0,0,0,((base)>>24) & 0xFF}

#define set_seg_null	(struct seg_descriptor){ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define setgate(gate, istrap, sel, off, dpl)			\
{								\
	(gate).gd_off_15_0 = (uint32_t) (off) & 0xffff;		\
	(gate).gd_sel = (sel);					\
	(gate).gd_args = 0;					\
	(gate).gd_rsv1 = 0;					\
	(gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;	\
	(gate).gd_s = 0;					\
	(gate).gd_dpl = (dpl);					\
	(gate).gd_p = 1;					\
	(gate).gd_off_31_16 = (uint32_t) (off) >> 16;		\
}


#endif/*_MINIOS_SYSTEM_H*/
