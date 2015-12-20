#include <fuckOS/assert.h>
#include <fuckOS/trap.h>
#include <fuckOS/time.h>
#include <fuckOS/lapic.h>
#include <fuckOS/sched.h>


void timer_handler(struct frame *tf)
{
	lapic_eoi();
	time_tick();
	schedule_tick();
}
