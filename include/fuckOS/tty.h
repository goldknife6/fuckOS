#ifndef _MINIOS_TTY_H
#define _MINIOS_TTY_H
#include <types.h>
#include <fuckOS/task.h>

#define TTY_IN_BYTES 256
#define TTY_OUT_BYTES 256
#define CONBUFSIZE 1024
#define NUMCON 3
#define NUMTTY 3

struct console_struct {
	uint32_t display_addr;	
	uint32_t original_start_addr;
	uint32_t current_start_addr;
	uint32_t cursor;
	uint32_t dismem_limit;


	uint32_t bgcolor;
	uint32_t fgcolor;
};

struct tty_queue {
	uint32_t data;
	uint32_t head;
	uint32_t tail;
	struct task_struct *proc_list;
	char buf[TTY_BUF_SIZE];
};

struct tty_struct {
	struct tty_queue read_q;
	struct tty_queue write_q
	void (*write)(struct tty_struct * tty);
};


extern struct console_struct console[NUMCON];
extern void console_init(uint32_t);
extern void console_write_char(uint32_t c);
extern void console_clear();
extern void updata_display_addr(uint32_t addr);
#endif/*_MINIOS_TTY_H*/
