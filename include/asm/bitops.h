#include <types.h>


static inline void 
set_bit(int nr, volatile uint8_t * addr)
{
    __asm__ __volatile__( "lock\n\r"
                "btsl %1,%0\n\r"
                :"+m" (*addr)
                :"Ir" (nr));
}

static inline void 
clear_bit(int nr, volatile uint8_t * addr)
{
        __asm__ __volatile__( "lock\n\r"
                "btrl %1,%0\n\r"
                :"+m" (*addr)
                :"Ir" (nr));
}

static inline int 
test_and_set_bit(int nr, volatile uint8_t * addr)
{
    int oldbit;

    __asm__ __volatile__( "lock\n\r"
                "btsl %2,%1\n\tsbbl %0,%0"
                :"=r" (oldbit),"+m" (*addr)
                :"Ir" (nr) : "memory");
        return oldbit;
}



