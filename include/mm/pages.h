#ifndef _MINIOS_PAGES_H_
#define _MINIOS_PAGES_H_ 

#ifdef CONFIG_PAE
# include "pgtable-3level.h"
#else
# include "pgtable-2level.h"
#endif/*!CONFIG_PAE*/

#include <asm/atomic.h>

#include <fuckOS/list.h>
#include <fuckOS/assert.h>

#include <mm/layout.h>

struct page {
	struct list_head list;
	atomic_t	nref;
	uint32_t	flags;
	int32_t		order;

	//struct kmem_cache* slab;
	void ** 	freelist;//slab
	int32_t		inuse;
	spinlock_t	slublock;
};

enum {
	_pg_free = 1 << 0,
	_pg_reserved = 1 << 1,
	_pg_slab = 1 << 2,
	_pg_buddy = 1 << 3,
	_pg_active = 1 << 4,
	_pg_locked = 1 << 5
};


// Page table/directory entry flags.
#define _PAGE_PRESENT   	0x001  
#define _PAGE_RW        	0x002
#define _PAGE_USER      	0x004
#define _PAGE_PWT       	0x008
#define _PAGE_PCD       	0x010
#define _PAGE_ACCESSED  	0x020
#define _PAGE_DIRTY     	0x040
#define _PAGE_PSE       	0x080  /* 4 MB (or 2MB) page, Pentium+, if present.. */
#define _PAGE_GLOBAL    	0x100  /* Global TLB entry PPro+ */

#define	PFN(x)			phys_to_pfn(x)

#define __pa(x) 		((physaddr_t)((x) - KERNEL_BASE_ADDR))
#define __va(x) 		((viraddr_t)((x) + KERNEL_BASE_ADDR))

#define v2p(kaddr) 		(virt_to_phys(__FILE__,__LINE__ ,kaddr))
#define p2v(paddr) 		(phys_to_virt(__FILE__,__LINE__ ,paddr))
#define pte2p(kaddr) 		(virt_to_phys(__FILE__,__LINE__ ,(uint32_t)kaddr))

#define page2pfn(page) 		(_page2pfn(__FILE__,__LINE__ ,(page)))
#define page2phys(page) 	(_page2phys(__FILE__,__LINE__ ,(page)))
#define page2virt(page) 	(_page2virt(__FILE__,__LINE__ ,(page)))

#define pfn2page(pfn) 		_pfn2page(__FILE__,__LINE__ ,pfn)
#define phys2page(phys) 	(_phys2page(__FILE__,__LINE__ ,(page)))
#define virt2page(virt) 	(_virt2page(__FILE__,__LINE__ ,(page)))

#define CHECK_ALIGN(a,n)					\
({								\
	bool _a ;_a = (ROUNDDOWN(a, n) == a);			\
})

#define ROUNDDOWN(a, n)						\
({								\
	viraddr_t __a = (viraddr_t) (a);			\
	(typeof(a)) (__a - __a % (n));				\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)						\
({								\
	viraddr_t __n = (viraddr_t) (n);				\
	(typeof(a)) (ROUNDDOWN((viraddr_t) (a) + __n - 1, __n));\
})

#define PAGE_ALIGN(x)		ROUNDDOWN(x, PAGE_SIZE)

extern pte_t *page_walk (pgd_t *pgd,viraddr_t address,bool create);
extern struct page *mempage;
extern int max_pfn;
extern int normal_maxpfn;
extern int high_maxpfn;
extern int totalram_pages;
extern pgd_t *kpgd;

static inline physaddr_t pte_val(volatile pte_t pte)
{
	return pte.pte_low;
}

static inline physaddr_t pmd_val(volatile pmd_t pmd)
{
	return pmd.pmd;
}

static inline physaddr_t pgd_val(volatile pgd_t pgd)
{
	return pgd.pgd;
}

static inline void pte_set(pte_t *pte,physaddr_t pa,int perm)
{
#ifdef CONFIG_DEBUG
	assert(pte);
	assert((pa & ~0xFFF) == pa);
#endif
	pte->pte_low = pa | perm;
}


static inline physaddr_t virt_to_phys(char *file,uint32_t line ,volatile viraddr_t address)
{

	if(address < KERNEL_BASE_ADDR)
		panic(file, line, "virt_to_page called with invalid pa %016llx", address);
	return __pa(address);
}
static inline viraddr_t phys_to_virt(char *file,uint32_t line ,volatile physaddr_t address)
{
	if(address > NORMAL_ADDR)
		panic(file, line, "phys_to_virt called with invalid pa %016llx", address);
	return __va(address);
}

static inline uint32_t pgd_index(viraddr_t address)
{
	return (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1));
}

static inline uint32_t pmd_index(viraddr_t address)
{
	return (((address) >> PMD_SHIFT) & (PTRS_PER_PMD-1));
}

static inline uint32_t pte_index(viraddr_t address)
{
	return (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE-1));
}

static inline viraddr_t pgd_page_vaddr(pgd_t pgd)
{
	return p2v(pgd_val(pgd) & PAGE_MASK);
}


static inline viraddr_t pmd_page_vaddr(pmd_t pmd)
{
	return p2v(pmd_val(pmd) & PAGE_MASK);
}

static inline viraddr_t pte_page_vaddr(pte_t pte)
{
	return p2v(pte_val(pte) & PAGE_MASK);
}

static inline pgd_t *_pgd_offset(pgd_t *pgd, viraddr_t address)
{
	assert(pgd);
	return pgd + pgd_index(address);
}
static inline pmd_t *_pmd_offset(pgd_t *pgd, viraddr_t address) 
{
	assert(pgd);
#ifdef _PAE_
    	return (pmd_t *)pgd_page_vaddr(*pgd) + pmd_index(address);
#else
	return (pmd_t *)pgd;
#endif
}
static inline pte_t *_pte_offset(pmd_t *pmd, viraddr_t address) 
{
	assert(pmd);
	return (pte_t *)pmd_page_vaddr(*pmd) + pte_index(address);
}

static inline pfn_t _page2pfn(char *file,uint32_t line ,volatile struct page * page)
{
	if(page < mempage || page >= mempage + max_pfn)
		panic(file, line, "page2pfn called with invalid page 0x%08x", page);
	return page - mempage;
}

static inline struct page *
_pfn2page(char *file,uint32_t line ,int pfn)
{
	if(pfn >= max_pfn)
		panic(file, line, "pfn2page called with invalid pfn 0x%08x", pfn);
	return mempage + pfn;
}
static inline struct page *
_virt2page(char *file,uint32_t line ,viraddr_t vir)
{

}
static inline struct page *
_phys2page(char *file,uint32_t line ,viraddr_t vir)
{

}


/*
*	计算物理页号
*/
static inline pfn_t phys_to_pfn(volatile physaddr_t address)
{
	return (address >> PAGE_SHIFT);
}
#endif/*!_MINIOS_PAGES_H_*/
