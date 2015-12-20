#include <fuckOS/lapic.h>
#include <fuckOS/trap.h>
#include <fuckOS/kernel.h>
#include <fuckOS/cpu.h>

#include <mm/pages.h>
#include <mm/layout.h>

#include <asm/x86.h>

#include <string.h>

physaddr_t lapicaddr; 
volatile uint32_t *lapic;

void lapicw(uint32_t index, uint32_t value)
{
	lapic[index] = value;
	lapic[LAPIC_ID];  // wait for write to finish, by reading
}

uint32_t lapicr(int32_t index)
{
	return lapic[index];
}


// Acknowledge interrupt.
void lapic_eoi(void)
{
	if (lapic)
		lapicw(LAPIC_EOI, 0);
}

static void
microdelay(int us)
{
	int i;
	for(i = 0;i < us;i++)
		;
}

void ap_startup()
{
	void *code;
	extern unsigned char mpentry_start[], mpentry_end[];
	
	code = (void *)p2v(MPENTRY_PADDR);
	memmove(code, mpentry_start, mpentry_end - mpentry_start);

		
	lapicw(LAPIC_ICRH , 0);
	lapicw(LAPIC_ICRL , DESTINATION_OTHERS | LEVEL_ASSERT |TRIGGER_MODE_EDGE | DELIVERY_MODE_INIT);
	while(lapic[LAPIC_ICRL] & DELIVERY_STATUS_BUSY)
		;

	lapicw(LAPIC_ICRH , 0);
	lapicw(LAPIC_ICRL , DESTINATION_OTHERS | LEVEL_ASSERT |TRIGGER_MODE_EDGE | DELIVERY_MODE_STARTUP|(MPENTRY_PADDR>>12));
	microdelay(200);
	lapicw(LAPIC_ICRH , 0);
	lapicw(LAPIC_ICRL , DESTINATION_OTHERS | LEVEL_ASSERT |TRIGGER_MODE_EDGE | DELIVERY_MODE_STARTUP|(MPENTRY_PADDR>>12));
}

void lapic_init()
{
	static char *base = (char*)KERNEL_MMIO;
	lapic = (uint32_t *)base;
	base += PAGE_SIZE;
	lapicw(LAPIC_SVR , LAPIC_SVR_ENABLE | IRQ_SPURIOUS);

	if(!bootcpu) {
		bootcpu = &cpus[get_cpuid()];
	}

	if(bootcpu != thiscpu) {
		lapicw(LAPIC_LINT0, MASKED);
	}

	lapicw(LAPIC_TIMER_DIVIDE, X1);
	lapicw(LAPIC_TIMER, PERIODIC | (IRQ_TIMER));
	lapicw(LAPIC_TIMER_INIT, 10000000); 

	lapicw(LAPIC_LINT1, MASKED);

	if (((lapic[LAPIC_VER]>>16) & 0xFF) >= 4)
		lapicw(LAPIC_PCINT, MASKED);

	lapicw(LAPIC_ERROR, IRQ_ERROR);

	lapicw(LAPIC_ESR, 0);
	lapicw(LAPIC_ESR, 0);


	lapicw(LAPIC_EOI, 0);
	
	//outb(0x22, 0x70);   // Select IMCR
	//outb(0x23, inb(0x23) | 1);  // Mask external interrupts.

	lapicw(LAPIC_TPR, 0);

}
