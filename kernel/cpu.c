#include <fuckOS/cpu.h>
#include <fuckOS/lapic.h>

struct cpuinfo cpus[CPUNUMS];

struct cpuinfo* bootcpu;

int get_cpuid()
{	
	if (lapic) {
		return lapic[LAPIC_ID] >> 24;
	}
	return 0;
}
