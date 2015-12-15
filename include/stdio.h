#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

#ifndef NULL
#define NULL	((void *) 0)
#endif /* !NULL */

void	cputchar(int c);
int	getchar(void);
int	iscons(int fd);

// kernel/printfmt.c
void	printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);
void	vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list);
int	snprintf(char *str, int size, const char *fmt, ...);
int	vsnprintf(char *str, int size, const char *fmt, va_list);

int 	vprintf(const char *fmt, va_list ap);

//lib/printf.c
int	printf(const char *fmt, ...);

#endif /* !_STDIO_H */
