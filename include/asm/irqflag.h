#ifndef _IRQFLAG_H
#define _IRQFLAG_H

#include <types.h>


static inline void local_irq_enable(void)
{
	__asm__ __volatile__("sti" : : : "memory","cc");
}

static inline void local_irq_disable(void)
{
	__asm__ __volatile__("cli" : : : "memory","cc");
}

static inline void local_irq_save(uint32_t flags)
{
	
}

static inline void local_irq_restore(uint32_t flags)
{
	
}

#endif/*!_IRQFLAG_H*/
