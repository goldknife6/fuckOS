#include <asm/atomic.h>

#include <types.h>

#include <mm/pages.h>
STATIC_INIT_SPIN_LOCK(pidlock);

/* max pid, equal to 2^15=32768 */
#define PID_MAX_DEFAULT 0x8000


#define BITS_PER_BYTE 8
//4k*8 32768
#define BITS_PER_PAGE (PAGE_SIZE * BITS_PER_BYTE)
//7fff 
//0111 1111 1111 1111
#define BITS_PER_PAGE_MASK (BITS_PER_PAGE - 1)


static pid_t last_pid = -1;
struct pidmap pidmap = { PID_MAX_DEFAULT, {0}};

static int 
test_and_set_bit(int offset, void *addr)
{
    uint32_t mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));
    uint32_t *p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));
    uint32_t old = *p;

    *p = old | mask; 

    return (old & mask) != 0;
}

static void 
clear_bit(int offset, void *addr)
{
    uint32_t mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));//取offset的后31位数据,并左移
    uint32_t *p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));//+优先级高于>>
    uint32_t old = *p;
    *p = old & ~mask;
}

static int 
find_next_zero_bit(void *addr, int size, int offset)
{
    uint32_t *p;
    uint32_t mask;

    while (offset < size)
    {
        p = ((uint32_t*)addr) + (offset >> (sizeof(uint32_t) + 1));
        mask = 1UL << (offset & (sizeof(uint32_t) * BITS_PER_BYTE - 1));

        if ((~(*p) & mask))
        {
            break;
        }
        ++offset;
    }

    return offset;
}

int 
alloc_pidmap()
{
	pid_t pid = last_pid + 1;
	int offset = pid & BITS_PER_PAGE_MASK;//把offset的最高为变为0，其他的不变
	spin_lock(&pidlock);
	if (!pidmap.nr_free)
	{
		spin_unlock(&pidlock);
		return -1;
	}

	offset = find_next_zero_bit(&pidmap.page, BITS_PER_PAGE, offset);

	if (BITS_PER_PAGE != offset && !test_and_set_bit(offset, &pidmap.page))
	{
		--pidmap.nr_free;
		last_pid = offset;
		spin_unlock(&pidlock);
		return offset;
	}
	spin_unlock(&pidlock);
   	return -1;
}

void 
free_pidmap(pid_t pid)
{
    	int offset = pid & BITS_PER_PAGE_MASK;
	spin_lock(&pidlock);
   	pidmap.nr_free++;
   	clear_bit(offset, &pidmap.page);
	spin_unlock(&pidlock); 
}
