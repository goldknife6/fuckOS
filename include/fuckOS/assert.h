#ifndef _MINIOS_ASSERT_H
#define _MINIOS_ASSERT_H
extern void _panic(const char *file, int line, const char *fmt,...)  __attribute__((noreturn));


#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)
#define __panic(x) panic("CPU:%d kernel panic at %s:%d: %s",get_cpuid(),__FILE__,__LINE__,x)
#define assert(x)		\
	do { if (!(x)) panic("assertion failed: %s", #x); } while (0)

#endif /*_MINIOS_ASSERT_H*/
