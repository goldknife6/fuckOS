#include <fuckOS/keyboard.h>
#include <fuckOS/keymap.h>
#include <fuckOS/trap.h>
#include <fuckOS/pic_8259A.h>
#include <fuckOS/tty.h>

#include <asm/x86.h>
#include <stdio.h>

kbdbuf_t kbdbuf;
int		code_with_E0;
int		shift_l;	/* l shift state	*/
int		shift_r;	/* r shift state	*/
int		alt_l;		/* l alt state		*/
int		alt_r;		/* r left state		*/
int		ctrl_l;		/* l ctrl state		*/
int		ctrl_r;		/* l ctrl state		*/
int		caps_lock;	/* Caps Lock		*/
int		num_lock;	/* Num Lock		*/
int		scroll_lock;	/* Scroll Lock		*/
int		column;

void keyboard_init()
{
	kbdbuf.count = 0;
	kbdbuf.p_head = kbdbuf.p_tail = kbdbuf.buf;

	enable_8259A_irq(IRQ_KBD);
	
}

uint8_t get_scancode_from_kdb()
{
	return inb(OUTPUTBUF);
	
}

void keyboard_read_to_buff()
{
	uint8_t  scancode = get_scancode_from_kdb();
	if (kbdbuf.count < KB_IN_BYTES) {
		*(kbdbuf.p_head) = scancode;
		kbdbuf.p_head++;
		if (kbdbuf.p_head == kbdbuf.buf + KB_IN_BYTES)
			kbdbuf.p_head = kbdbuf.buf;
		kbdbuf.count++;
	}

}

uint8_t get_byte_from_kb_buf()
{
	uint8_t	scan_code;

	//while (kbdbuf.count <= 0) {} /* wait for a byte to arrive */

	scan_code = *(kbdbuf.p_tail);
	kbdbuf.p_tail++;
	if (kbdbuf.p_tail == kbdbuf.buf + KB_IN_BYTES) {
		kbdbuf.p_tail = kbdbuf.buf;
	}
	kbdbuf.count--;

	return scan_code;
}


void keyboard_read()
{
	uint8_t		scan_code;
	int		make = 0;
	uint32_t	key = 0;
	uint32_t*	keyrow;
	
	while (kbdbuf.count > 0) {
		code_with_E0 = 0;
		scan_code = get_byte_from_kb_buf();
		
		/* parse the scan code below */
		if (scan_code == 0xE1) {
			int i;
			uint8_t pausebreak_scan_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
			int is_pausebreak = 1;
			for (i = 1; i < 6; i++) {
				if (get_byte_from_kb_buf() != pausebreak_scan_code[i]) {
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0) {
			code_with_E0 = 1;
			scan_code = get_byte_from_kb_buf();

			/* PrintScreen is pressed */
			if (scan_code == 0x2A) {
				code_with_E0 = 0;
				if ((scan_code = get_byte_from_kb_buf()) == 0xE0) {
					code_with_E0 = 1;
					if ((scan_code = get_byte_from_kb_buf()) == 0x37) {
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}
			/* PrintScreen is released */
			else if (scan_code == 0xB7) {
				code_with_E0 = 0;
				if ((scan_code = get_byte_from_kb_buf()) == 0xE0) {
					code_with_E0 = 1;
					if ((scan_code = get_byte_from_kb_buf()) == 0xAA) {
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}
		}

		if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
			int caps;

			/* make or break */
			make = (scan_code & FLAG_BREAK ? 0 : 1);
			
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

			column = 0;

			caps = shift_l || shift_r;
			if (caps_lock &&
			    keyrow[0] >= 'a' && keyrow[0] <= 'z')
				caps = !caps;

			if (caps)
				column = 1;

			if (code_with_E0)
				column = 2;

			key = keyrow[column];

			switch(key) {
			case SHIFT_L:
				shift_l	= make;
				break;
			case SHIFT_R:
				shift_r	= make;
				break;
			case CTRL_L:
				ctrl_l	= make;
				break;
			case CTRL_R:
				ctrl_r	= make;
				break;
			case ALT_L:
				alt_l	= make;
				break;
			case ALT_R:
				alt_l	= make;
				break;
			case CAPS_LOCK:
				if (make) {
					caps_lock   = !caps_lock;
					//set_leds();
				}
				break;
			case NUM_LOCK:
				if (make) {
					num_lock    = !num_lock;
					//set_leds();
				}
				break;
			case SCROLL_LOCK:
				if (make) {
					scroll_lock = !scroll_lock;
					//set_leds();
				}
				break;
			default:
				break;
			}
		}

		if(make){ /* Break Code is ignored */
			int pad = 0;

			/* deal with the numpad first */
			if ((key >= PAD_SLASH) && (key <= PAD_9)) {
				pad = 1;
				switch(key) {	/* '/', '*', '-', '+',
						 * and 'Enter' in num pad
						 */
				case PAD_SLASH:
					key = '/';
					break;
				case PAD_STAR:
					key = '*';
					break;
				case PAD_MINUS:
					key = '-';
					break;
				case PAD_PLUS:
					key = '+';
					break;
				case PAD_ENTER:
					key = ENTER;
					break;
				default:
					/* the value of these keys
					 * depends on the Numlock
					 */
					if (num_lock) {	/* '0' ~ '9' and '.' in num pad */
						if (key >= PAD_0 && key <= PAD_9)
							key = key - PAD_0 + '0';
						else if (key == PAD_DOT)
							key = '.';
					}
					else{
						switch(key) {
						case PAD_HOME:
							key = HOME;
							break;
						case PAD_END:
							key = END;
							break;
						case PAD_PAGEUP:
							key = PAGEUP;
							break;
						case PAD_PAGEDOWN:
							key = PAGEDOWN;
							break;
						case PAD_INS:
							key = INSERT;
							break;
						case PAD_UP:
							key = UP;
							break;
						case PAD_DOWN:
							key = DOWN;
							break;
						case PAD_LEFT:
							key = LEFT;
							break;
						case PAD_RIGHT:
							key = RIGHT;
							break;
						case PAD_DOT:
							key = DELETE;
							break;
						default:
							break;
						}
					}
					break;
				}
			}
			key |= shift_l	? FLAG_SHIFT_L	: 0;
			key |= shift_r	? FLAG_SHIFT_R	: 0;
			key |= ctrl_l	? FLAG_CTRL_L	: 0;
			key |= ctrl_r	? FLAG_CTRL_R	: 0;
			key |= alt_l	? FLAG_ALT_L	: 0;
			key |= alt_r	? FLAG_ALT_R	: 0;
			key |= pad	? FLAG_PAD	: 0;
			
			handle_key(key);
		}
	}
	
}
