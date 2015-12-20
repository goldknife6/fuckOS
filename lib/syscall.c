#include <syscall.h>
#include <lib.h>

static inline int32_t
syscall(int num, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret;

	asm volatile("int %1\n"
		: "=a" (ret)
		: "i" (0x80),
		  "a" (num),
		  "d" (a1),
		  "c" (a2),
		  "b" (a3),
		  "D" (a4),
		  "S" (a5)
		: "cc", "memory");

	return ret;
}

void 
sys_cputs(const char *s, size_t len)
{
	syscall(SYS_CPUTS,(uint32_t)s, len, 0, 0, 0);
}

int 
sys_exit(pid_t pid)
{
	return syscall(SYS_EXIT,(uint32_t)pid,0,0,0,0);
}
pid_t 
sys_getpid()
{
	return syscall(SYS_GETPID,0,0,0,0,0);
}

pid_t 
sys_exofork(void)
{
	return syscall(SYS_EXOFORK,0,0,0,0,0);
}

