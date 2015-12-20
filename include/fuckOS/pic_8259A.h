#ifndef _MINIOS_8259A_H
#define _MINIOS_8259A_H

/* i8259A PIC registers */
#define PIC_MASTER_CMD          0x20
#define PIC_MASTER_IMR          0x21
#define PIC_MASTER_ISR          PIC_MASTER_CMD
#define PIC_MASTER_POLL         PIC_MASTER_ISR
#define PIC_MASTER_OCW3         PIC_MASTER_ISR
#define PIC_SLAVE_CMD           0xA0
#define PIC_SLAVE_IMR           0xA1
#define PIC_CASCADE_IR          2






void init_8259A(void);
void setmask_8259A_irq(uint16_t mask);
void disable_8259A_irq(unsigned int irq);
void enable_8259A_irq(unsigned int irq);
void irq_eoi(void);
#endif/*_MINIOS*/
