#ifndef _ATOMIC_H
#define _ATOMIC_H

#include <asm/x86.h>
#define ATOMIC_INIT(i)  		{ (i) }  
#define INIT_SPIN_LOCK			{0}
#define STATIC_INIT_SPIN_LOCK(lock)	static spinlock_t lock = {0};
typedef struct{
	volatile int counter;
}atomic_t;

typedef struct spinlock{
	volatile uint32_t locked;       // Is the lock held?
}spinlock_t;

static inline void spin_lock(struct spinlock *lk)
{
	while (xchg(&lk->locked, 1) != 0)
		asm volatile ("pause");
}

static inline void spin_unlock(struct spinlock *lk)
{
	xchg(&lk->locked, 0);
}

static inline int spin_trylock(struct spinlock *lk)
{
	return !xchg(&lk->locked, 1);
	
}

static inline void SPIN_LOCK_INIT(struct spinlock *lk)
{
	lk->locked = 0;
}

static inline void INIT_ATOMIC(atomic_t *at)
{
	at->counter = 0;
}


static inline int atomic_read(const atomic_t *v)  
{  
    return v->counter;  
}

static inline void atomic_set(atomic_t *v, int i)  
{  
    v->counter = i;  
} 

static inline void atomic_add(int i, atomic_t *v)  
{  
    asm volatile("lock addl %1,%0"  
             : "+m" (v->counter)  
             : "ir" (i));  
}

static inline void atomic_sub(int i, atomic_t *v)  
{  
    asm volatile("lock subl %1,%0"  
             : "+m" (v->counter)  
             : "ir" (i));  
}  

static inline void atomic_dec(atomic_t *v)  
{  
    asm volatile("lock decl %0"  
             : "+m" (v->counter));  
} 

static inline void atomic_inc(atomic_t *v)  
{  
    asm volatile("lock incl %0"  
             : "+m" (v->counter));  
}  

static inline int atomic_dec_and_test(atomic_t *v)  
{  
    unsigned char c;  
  
    asm volatile("lock decl %0; sete %1"  
             : "+m" (v->counter), "=qm" (c)  
             : : "memory");  
    return c != 0;  
}  

static inline int atomic_inc_and_test(atomic_t *v)  
{  
    unsigned char c;  
  
    asm volatile("lock incl %0; sete %1"  
             : "+m" (v->counter), "=qm" (c)  
             : : "memory");  
    return c != 0;  
} 

static inline int atomic_sub_and_test(int i, atomic_t *v)  
{  
    unsigned char c;  
  
    asm volatile("lock subl %2,%0; sete %1"  
             : "+m" (v->counter), "=qm" (c)  
             : "ir" (i) : "memory");  
    return c;  
}  

#endif/*!_ATOMIC_H*/
