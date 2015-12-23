#ifndef _MINIOS_PIDMAP_H
#define _MINIOS_PIDMAP_H


/* max pid, equal to 2^15=32768 */
#define PID_MAX_DEFAULT 0x8000


#define BITS_PER_BYTE 8
//4k*8 32768
#define BITS_PER_PAGE (PAGE_SIZE * BITS_PER_BYTE)
//7fff 
//0111 1111 1111 1111
#define BITS_PER_PAGE_MASK (BITS_PER_PAGE - 1)


extern int alloc_pidmap();
extern void free_pidmap(pid_t);

#endif
