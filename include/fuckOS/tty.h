#ifndef _MINIOS_TTY_H
#define _MINIOS_TTY_H
#include <types.h>

#define TTY_IN_BYTES 256
#define TTY_OUT_BYTES 256
#define CONBUFSIZE 1024
#define NUMCON 3
#define NUMTTY 3

typedef struct console_struct {
	uint32_t* display_addr;	
	uint32_t* original_start_addr;
	uint32_t* current_start_addr;
	uint32_t cursor;
	uint32_t dismem_limit;


	uint32_t bgcolor;
	uint32_t fgcolor;
	
	uint32_t conbuf[CONBUFSIZE];	
}console_t;


typedef struct tty_struct {
	uint32_t in_buf[TTY_IN_BYTES];
	uint32_t out_buf[TTY_OUT_BYTES];
	console_t* console_p;
}tty_t;


extern console_t console[];
extern void console_init();
extern void console_write_char(uint32_t c);
extern void console_clear();
#endif/*_MINIOS_TTY_H*/
