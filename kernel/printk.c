#include <fuckOS/kernel.h>
#include <fuckOS/tty.h>

#include <asm/atomic.h>


#include <types.h>

int vprintf(const char *fmt, va_list ap);
void vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list ap);


int printk(const char *fmt, ...)
{
	
	va_list ap;
	int cnt;
	va_start(ap, fmt);
	cnt = vprintf(fmt, ap);
	va_end(ap);
	
	return cnt;
}


static void putch(int ch, int *cnt)
{
	console_write_char(ch);
	*cnt++;
}

int vprintf(const char *fmt, va_list ap)
{
	int cnt = 0;
	STATIC_INIT_SPIN_LOCK(pklock);
	spin_lock(&pklock);
	vprintfmt((void*)putch, &cnt, fmt, ap);
	spin_unlock(&pklock);
	return cnt;
}


