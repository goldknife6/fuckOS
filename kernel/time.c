#include <fuckOS/cpu.h>
#include <fuckOS/time.h>

static uint32_t jiffies;

void
time_init()
{
	jiffies = 0;
}


void
time_tick()
{
	if(thiscpu != bootcpu)
		return;
	jiffies++;
}

uint32_t
time_msec()
{
	return jiffies * 10;
}

uint32_t
get_jiffies()
{
	return jiffies;
}
