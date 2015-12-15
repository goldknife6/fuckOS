
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)


SECTIONS
{

	. = 0xC0100000;

	.text :  AT(0x100000) {
		*(.text  .text.* .stub .gnu.linkonce.t.*)
	}

	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}


	. = ALIGN(0x1000);

	/* The data segment */

	PROVIDE(_DATA_START = .);
	.data : {
		*(.data)
	}
	PROVIDE(_DATA_END = .);

	. = ALIGN(0x1000);

	PROVIDE(_BSS_START = .);
	.bss : {
		*(.bss)
	}
	PROVIDE(_BSS_END = .);

	/DISCARD/ : {
		*(.eh_frame .note.GNU-stack)
	}

}
