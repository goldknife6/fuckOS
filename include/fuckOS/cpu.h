#ifndef _MINIOS_CPU_H
#define _MINIOS_CPU_H
#include <types.h>

#include <sys/system.h>
#include <fuckOS/task.h>
#define	CPUNUMS	256

enum {
	CPU_UNUSED = 0,
	CPU_STARTED,
	CPU_HALTED,
};

struct cpuinfo {
	uint8_t cpu_id;                 // Local APIC ID; index into cpus[] below
	volatile unsigned cpu_status;   // The status of the CPU
	struct task_struct *cpu_task;   // The currently-running environment.
	struct taskstate cpu_ts;        // Used by x86 to find stack for interrupt
};



extern struct cpuinfo cpus[CPUNUMS];
extern int ncpu;                    // Total number of CPUs in the system
extern struct cpuinfo* bootcpu;     		// The boot-strap processor (BSP)
extern physaddr_t lapicaddr;        // Physical MMIO address of the local APIC


#define thiscpu (&cpus[get_cpuid()])


extern void detect_cpuinfo();
static int get_cpuid()
{
	return 0;
}
#endif/*!_MINIOS_CPU_H*/
