#include <fuckOS/assert.h>
#include <fuckOS/keyboard.h>
#include <fuckOS/tty.h>

int current_tty;
struct tty_struct tty;

int tty_read(int num, int *buf, int nr)
{
	int i;
	int *start,*end;
	
	start = &tty.read_q.buf[tty.read_q.head];
	end = &tty.read_q.buf[tty.read_q.tail];

	for (i = 0; i < nr; i++) {
		if (start == end)
			break;
		*(buf++) = *(start++);
		tty.read_q.head++;
	}
	return i;
}

int tty_write(int num, char *buf, int nr)
{
	printk("%.*s",nr,buf);
	return nr;
}

void handle_key(uint32_t key)
{
	if (!(key & FLAG_EXT)) {
		if (key & FLAG_CTRL_L) switch (key & 0xff) {
			case 'd' :
				key = -EEOF;goto inkey;
			default:
				return;
		} else {
			console_write_char(key & 0xFF);
		}
	} else {
		int raw_code = key & MASK_RAW;
		
		switch(raw_code) {
		case ENTER:
			console_write_char('\n');
			key = '\n';
			goto inkey;
		case BACKSPACE:
			console_write_char('\b');
			key = '\b';
			goto inkey;
		case UP:
		/*
			if ((key & FLAG_SHIFT_L) ||
			    (key & FLAG_SHIFT_R)) {	// Shift + Up 
				scroll_screen(tty->console, SCR_DN);
			}
		*/
			break;
		case DOWN:
		/*
			if ((key & FLAG_SHIFT_L) ||
			    (key & FLAG_SHIFT_R)) {	// Shift + Down 
				scroll_screen(tty->console, SCR_UP);
			}
		*/
			break;
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			
			//if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) 	/* Alt + F1~F12 */
				//if(raw_code - F1 >= NUM_CONS || raw_code - F1 < 0)
				//	return;
				//curtty = &tty[raw_code - F1];
				//cons_select(raw_code - F1);
			
			break;
		default:
			break;
		}
		return;
	}
inkey:
	tty.read_q.buf[tty.read_q.tail] = key;
	if (++tty.read_q.tail >= TTY_BUF_SIZE)
		tty.read_q.tail = 0;
}

