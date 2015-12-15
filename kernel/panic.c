#include <fuckOS/kernel.h>

#include <asm/atomic.h>

const char *panicstr;
extern int vprintf(const char *fmt, va_list ap);


void _panic(const char *file, int line, const char *fmt,...)
{
	extern spinlock_t pklock;
	va_list ap;
	if (panicstr)
		goto dead;
	panicstr = fmt;

	// Be extra sure that the machine is in as reasonable state
	__asm __volatile("cli; cld");
	

	va_start(ap, fmt);
	printk("kernel panic at %s:%d: ", file, line);
	vprintf(fmt, ap);
	printk("\n");
	va_end(ap);
dead:
	__asm __volatile("L1:hlt; jmp L1");
}
