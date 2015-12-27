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

    __asm__ __volatile__( "lock\n\t"
                "btsl %2,%1\n\tsbbl %0,%0"
                :"=r" (oldbit),"+m" (*addr)
                :"Ir" (nr) : "memory");
        return oldbit;
}

#define find_first_zero(addr) ({\
        int __res;\
        __asm__ __volatile__("cld\n"\
            "1:\tlodsl\n\t"\
            "notl %%eax\n\t"\
            "bsfl %%eax,%%edx\n\t"\
            "je 2f\n\t"\
            "addl %%edx,%%ecx\n\t"\
            "jmp 3f\n"\
            "2:\t addl $32,%%ecx\n\t"\
            "cmpl $8192,%%ecx\n\t"\
            "jl 1b\n"\
            "3:"\
            :"=c"(__res):"c"(0),"S"(addr));\
        __res;})

