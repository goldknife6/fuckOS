#include <asm/x86.h>
#include <fuckOS/kernel.h>
#include <fuckOS/pic_8259A.h>
#include <fuckOS/trap.h>


uint16_t irq_mask_8259A =  0xFFFF;



void init_8259A()
{

	outb(PIC_MASTER_IMR, 0xff);     	/* mask all of 8259A-1 */
	io_delay();
	outb(PIC_SLAVE_IMR,  0xff);      	/* mask all of 8259A-2 */
	io_delay();


	outb(PIC_MASTER_CMD,0x11); 		/* ICW1: select 8259A-1 init */
	io_delay();
	
	outb(PIC_MASTER_IMR, IRQ0_VECTOR);	/* ICW2: 8259A-1 IR0-7 mapped to to 0x20-0x27*/
	io_delay();

	/* 8259A-1 (the master) has a slave on IR2 */
	outb(PIC_MASTER_IMR,1U << PIC_CASCADE_IR);/*ICM3*/
	io_delay();
	outb(PIC_MASTER_IMR,0x1);		/* ICW4 */
	io_delay();

	//set up slave

	outb(PIC_SLAVE_CMD,0x11); 		/* ICW1: select 8259A-1 init */
	io_delay();
	outb(PIC_SLAVE_IMR, IRQ8_VECTOR);	/* ICW2: 8259A-1 IR0-7 mapped to to 0x28-0x2F*/
	io_delay();

	/* 8259A-2 (the slave) has a slave on IR2 */
	outb(PIC_SLAVE_IMR,PIC_CASCADE_IR);/*ICM3*/
	io_delay();
	outb(PIC_SLAVE_IMR,0x1);		/* ICW4 */
	io_delay();
	
	if (irq_mask_8259A != 0xFFFF)
		setmask_8259A_irq(irq_mask_8259A);
	
}

void setmask_8259A_irq(uint16_t mask)
{
	int i;
	irq_mask_8259A = mask;
        outb(PIC_MASTER_IMR,(char)mask);
	io_delay();
        outb(PIC_SLAVE_IMR,(char)(mask>>8));
	io_delay();
	printk("enabled interrupts:");
	for (i = 0; i < 16; i++)
		if (~mask & (1<<i))
			printk(" %d", i);
	printk("\n");

}

void disable_8259A_irq(unsigned int irq)
{
       	int i;
        unsigned int mask = 1 << irq;
        irq_mask_8259A  |= mask;

        outb(PIC_MASTER_IMR,(char)irq_mask_8259A);
	io_delay();
        outb(PIC_SLAVE_IMR,(char)(irq_mask_8259A>>8));
	io_delay();
	
}

void enable_8259A_irq(unsigned int irq)
{
	setmask_8259A_irq(irq_mask_8259A & ~(1<<irq));
}

void irq_eoi()
{
	// OCW2: rse00xxx
	//   r: rotate
	//   s: specific
	//   e: end-of-interrupt
	// xxx: specific interrupt line
	
	outb(PIC_MASTER_CMD, 0x20);
	io_delay();
	outb(PIC_SLAVE_CMD, 0x20);
	io_delay();
}

