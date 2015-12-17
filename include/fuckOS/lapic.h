#ifndef _MINIOS_LAPIC_CPU_H
#define _MINIOS_LAPIC_CPU_H
#include <types.h>

//Address 	Register Name 				Software Read/Write
//FEE0 0000H	Reserved 
//FEE0 0010H	Reserved 
//FEE0 0020H	Local APIC ID Register 			Read/Write.
#define LAPIC_ID      (0x0020/4)   // ID
//FEE0 0030H	Local APIC Version Register 		Read Only.
#define LAPIC_VER     (0x0030/4)   // Version
//FEE0 0040H	Reserved 
//FEE0 0050H	Reserved 
//FEE0 0060H	Reserved 
//FEE0 0070H	Reserved 
//FEE0 0080H	Task Priority Register (TPR) Register	Read/Write.
#define LAPIC_TPR     (0x0080/4)   // Task Priority
//FEE0 0090H	Arbitration Priority 
//FEE0 00A0H	Processor Priority Register (PPR) 	Read Only.
//FEE0 00B0H	EOI Register 				Write Only.
#define LAPIC_EOI     (0x00B0/4)   // EOI
//FEE0 00C0H	Reserved 
//FEE0 00D0H	Logical Destination Register 		Read/Write.
//FEE0 00E0H	Destination Format Register 		Bits 0-27 Read only bits 28-31 Read/Write.
//FEE0 00F0H	Spurious Interrupt Vector Register 	Bits 0-8 Read/Write; bits 9-31 Read Only.
#define LAPIC_SVR     (0x00F0/4)   // Spurious Interrupt Vector
	#define LAPIC_SVR_ENABLE     0x00000100   // Unit Enable
//FEE0 0100H	In-Service Register (ISR); 		bits 0:31 Read Only.
//FEE0 0110H	In-Service Register (ISR); 		bits 32:63 Read Only.
//FEE0 0120H	In-Service Register (ISR); 		bits 64:95 Read Only.
//FEE0 0130H	In-Service Register (ISR); 		bits 96:127 Read Only.
//FEE0 0140H	In-Service Register (ISR); 		bits 128:159 Read Only.
//FEE0 0150H	In-Service Register (ISR); 		bits 160:191 Read Only.
//FEE0 0160H	In-Service Register (ISR); 		bits 192:223 Read Only.
//FEE0 0170H	In-Service Register (ISR); 		bits 224:255 Read Only.
//FEE0 0180H	Trigger Mode Register (TMR); 		bits 0:31 Read Only.
//FEE0 0180H	Trigger Mode Register (TMR); 		bits 32:63 Read Only.
//FEE0 01A0H	Trigger Mode Register (TMR); 		bits 64:95 Read Only.
//FEE0 01B0H	Trigger Mode Register (TMR); 		bits 96:127 Read Only.
//FEE0 01C0H	Trigger Mode Register (TMR); 		bits 128:159 Read Only.
//FEE0 01D0H	Trigger Mode Register (TMR); 		bits 160:191 Read Only.
//FEE0 01E0H	Trigger Mode Register (TMR); 		bits 192:223 Read Only.
//FEE0 01F0H	Trigger Mode Register (TMR); 		bits 224:255 Read Only.
//FEE0 0200H	Interrupt Request Register (IRR); 	bits 0:31 Read Only.
//FEE0 0210H	Interrupt Request Register (IRR); 	bits 32:63 Read Only.
//FEE0 0220H	Interrupt Request Register (IRR); 	bits 64:95 Read Only.
//FEE0 0230H	Interrupt Request Register (IRR); 	bits 96:127 Read Only.
//FEE0 0240H	Interrupt Request Register (IRR); 	bits 128:159 Read Only.
//FEE0 0250H	Interrupt Request Register (IRR); 	bits 160:191 Read Only.
//FEE0 0260H	Interrupt Request Register (IRR); 	bits 192:223 Read Only.
//FEE0 0270H	Interrupt Request Register (IRR); 	bits 224:255 Read Only.
//FEE0 0280H	Error Status Register 
#define LAPIC_ESR     (0x0280/4)   // Error Status
//FEE0 0290H through FEE0 02F0H	Reserved 
//FEE0 0300H	Interrupt Command Register (ICR); 	bits 0-31 Read/Write.
#define LAPIC_ICRL   (0x0300/4)   // Interrupt Command
	#define DELIVERY_MODE_INIT       	0x00000500   // INIT/RESET
	#define DELIVERY_MODE_STARTUP    	0x00000600   // Startup IPI
	#define DELIVERY_MODE_FIXED      	0x00000000

	#define DELIVERY_STATUS    		0x00001000   // Delivery status
	#define DELIVERY_STATUS_BUSY       	0x00001000

	#define LEVEL_ASSERT     		0x00004000   // Assert interrupt (vs deassert)
	#define LEVEL_DEASSERT   		0x00000000

	#define TRIGGER_MODE_LEVEL      	0x00008000   // Level triggered
	#define TRIGGER_MODE_EDGE      		0x00000000   // Edge triggered

	#define DESTINATION_BCAST      		0x00080000   // Send to all APICs, including self.
	#define DESTINATION_OTHERS     		0x000C0000   // Send to all APICs, excluding self.
//FEE0 0310H	Interrupt Command Register (ICR); 	bits 32-63 Read/Write.
#define LAPIC_ICRH	(0x0310/4)   // Interrupt Command

//FEE0 0320H	LVT Timer Register 			Read/Write.
#define LAPIC_TIMER	(0x0320/4)   // Local Vector Table 0 (TIMER)
	#define X1         0x0000000B   // divide counts by 1
	#define PERIODIC   0x00020000   // Periodic
//FEE0 0330H	LVT Thermal Sensor 			Read/Write.
//FEE0 0340H 	LVT Performance Monitoring Counters Register 	Read/Write.
#define LAPIC_PCINT   (0x0340/4)   // Performance Counter LVT

//FEE0 0350H 	LVT LINT0 Register 			Read/Write.
#define LAPIC_LINT0	(0x0350/4) 

//FEE0 0360H 	LVT LINT1 Register 			Read/Write.
#define LAPIC_LINT1	(0x0360/4) 

//FEE0 0370H	LVT Error Register Read/Write.
#define LAPIC_ERROR	(0x0370/4)   // Local Vector Table 3 (ERROR)
	#define MASKED     0x00010000   // Interrupt masked

//FEE0 0380H	Initial Count Register (for Timer) 	Read/Write.
#define LAPIC_TIMER_INIT   (0x0380/4)
//FEE0 0390H	Current Count Register (for Timer) 	Read Only.
#define LAPIC_TIMER_CURR   (0x0390/4)
//FEE0 03A0H	through FEE0 03D0H Reserved 

//FEE0 03E0H	Divide Configuration Register (for Timer) Read/Write.
#define LAPIC_TIMER_DIVIDE   (0x03E0/4)
//FEE0 03F0H	Reserved

extern physaddr_t lapicaddr;

extern void lapic_init(void);
extern void lapic_startup(void);
extern volatile uint32_t *lapic;
extern void lapicw(uint32_t index, uint32_t value);
extern uint32_t lapicr(int32_t index);
extern void lapic_eoi(void);

#endif/*_MINIOS_LAPIC_CPU_H*/
