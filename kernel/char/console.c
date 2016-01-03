#include <types.h>
#include <string.h>
#include <fuckOS/tty.h>

#include <mm/layout.h>

struct console_struct console;
static void vga_graphic_putc(uint32_t c);
static void vga_text_putc(uint32_t c);

#define CGA_BASE	0x3D4
#define CRT_ROWS	25
#define CRT_COLS	80
#define CRT_SIZE	(CRT_ROWS * CRT_COLS)
#define crt_pos 	(console.cursor)

#define crt_buf  	((uint16_t *)console.display_addr)
void console_init(viraddr_t dispmem)
{
	int i,pos;
	console.display_addr = dispmem;
	console.original_start_addr = dispmem;
	console.current_start_addr = dispmem;
	console.cursor = 0;
	memset(&tty,0,sizeof(tty));
	console_clear();

}

void console_write_char(uint32_t c)
{
	vga_text_putc(c);
}

void updata_display_addr(uint32_t addr)
{
	console.display_addr = addr;
}



void console_clear()
{
	int i = 0;
	for(i;i<CRT_SIZE;i++)
		crt_buf[i] = 0;
}


static void vga_text_putc(uint32_t c)
{
	if (!(c & ~0xFF))
		c |= 0x0700;
	
	switch (c & 0xff) {
	case '\b':
		if (crt_pos > 0) {
			crt_pos--;
			crt_buf[crt_pos] = (c & ~0xff) | ' ';
		}
		break;
	case '\n':
		crt_pos += CRT_COLS;
	case '\r':
		crt_pos -= (crt_pos % CRT_COLS);
		break;
	case '\t':
		vga_text_putc(' ');
		vga_text_putc(' ');
		vga_text_putc(' ');
		vga_text_putc(' ');
		vga_text_putc(' ');
		break;
	default:
		crt_buf[crt_pos++] = c;		// write the character 
		break;
	}

	if (crt_pos >= CRT_SIZE) {
		int i;

		memmove(crt_buf, crt_buf + CRT_COLS, (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
		for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
			crt_buf[i] = 0x0700 | ' ';
		crt_pos -= CRT_COLS;
	}


	/* move that little blinky thing */
	outb(CGA_BASE, 14);
	outb(CGA_BASE + 1, 100 >> 8);
	outb(CGA_BASE, 15);
	outb(CGA_BASE + 1, 100);
}


