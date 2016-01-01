#include <fuckOS/assert.h>
#include <fuckOS/trap.h>
#include <fuckOS/keyboard.h>
#include <fuckOS/pic_8259A.h>

void kbd_handler(struct frame *tf)
{
	irq_eoi();
	keyboard_read_to_buff();
	keyboard_read();
}
