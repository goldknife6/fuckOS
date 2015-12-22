#ifndef _MINIOS_PAGES_H_
#define _MINIOS_PAGES_H_ 

#ifdef CONFIG_PAE
# include "pgtable-3level.h"
#else
# include "pgtable-2level.h"
#endif

#include <asm/atomic.h>

#include <fuckOS/list.h>
#include <fuckOS/assert.h>

#include <mm/mm.h>
#include <mm/layout.h>
#include <mm/slab.h>

struct page {
	struct list_head list;
	atomic_t	nref;
	uint32_t	flags;
	int32_t		order;

	struct mem_cache* slab;
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

typedef enum {
	_GFP_HIGH = 1 << 0,
	_GFP_ZERO = 1 << 1,
	_GFP_NONE = 1 << 2
}gfp_t;


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

#define v2p(kaddr) 		virt_to_phys(__FILE__,__LINE__ ,(viraddr_t)(kaddr))
#define p2v(paddr) 		phys_to_virt(__FILE__,__LINE__ ,(physaddr_t)(paddr))
#define pgd2p(kaddr) 		virt_to_phys(__FILE__,__LINE__ ,(uint32_t)(kaddr))

#define page2pfn(page) 		_page2pfn(__FILE__,__LINE__ ,(page))
#define page2phys(page) 	_page2phys(__FILE__,__LINE__ ,(page))
#define page2virt(page) 	_page2virt(__FILE__,__LINE__ ,(page))

#define pfn2page(pfn) 		_pfn2page(__FILE__,__LINE__ ,pfn)
#define phys2page(phys) 	_phys2page(__FILE__,__LINE__ ,phys)
#define virt2page(virt) 	_virt2page(__FILE__,__LINE__ ,virt)

#define pte_set(pte,pa,perm)	_pte_set(__FILE__,__LINE__ ,pte,pa,perm)
#define pmd_set(pmd,pa,perm)	_pmd_set(__FILE__,__LINE__ ,pmd,pa,perm)
#define pgd_set(pgd,pa,perm)	_pgd_set(__FILE__,__LINE__ ,pgd,pa,perm)



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

static inline void _pte_set(char *file,uint32_t line ,
				pte_t *pte,physaddr_t pa,int perm)
{
#ifdef CONFIG_DEBUG
	if (!pte)
		_panic(file,line,"pte_set pte == NULL\n");
	if ((pa & ~0xFFF) != pa)
		_panic(file,line,"pte_set pa:%"PLEN"x not PAGE_SIZE aligned\n",pa);
	if ((perm & 0xFFF) != perm)
		_panic(file,line,"pte_set perm:%08x not valid\n",perm);
#endif
	pte->pte_low = pa | perm;
}

static inline void _pmd_set(char *file,uint32_t line ,
				pmd_t *pmd,physaddr_t pa,int perm)
{
#ifdef CONFIG_DEBUG
	if (!pmd)
		_panic(file,line,"pmd_set pmd == NULL\n");
	if ((pa & ~0xFFF) != pa)
		_panic(file,line,"pmd_set pa:%"PLEN"x not PAGE_SIZE aligned\n",pa);
	if ((perm & 0xFFF) != perm)
		_panic(file,line,"pmd_set perm:%08x not valid\n",perm);
#endif
	pmd->pmd = pa | perm;
}

static inline void _pgd_set(char *file,uint32_t line ,
				pgd_t *pgd,physaddr_t pa,int perm)
{
#ifdef CONFIG_DEBUG
	if (!pgd)
		_panic(file,line,"pgd_set pgd == NULL\n");
	if ((pa & ~0xFFF) != pa)
		_panic(file,line,"pgd_set pa:%"PLEN"x not PAGE_SIZE aligned\n",pa);
	if ((perm & 0xFFF) != perm)
		_panic(file,line,"pgd_set perm:%08x not valid\n",perm);
#endif
	pgd->pgd = pa | perm;
}
/*
*	计算物理页号
*/
static inline pfn_t 
phys_to_pfn(volatile physaddr_t address)
{
	return (address >> PAGE_SHIFT);
}

static inline physaddr_t 
virt_to_phys(char *file,uint32_t line ,volatile viraddr_t address)
{

	if(address < KERNEL_BASE_ADDR) {
		_panic(file, line, "v2p called with invalid virt %"PLEN"x", address);
	}
	return __pa(address);
}

static inline viraddr_t 
phys_to_virt(char *file,uint32_t line ,volatile physaddr_t address)
{
	if(address >= NORMAL_ADDR) {
		_panic(file, line, "p2v called with invalid phys %"PLEN"x", address);
	}
	return __va(address);
}

static inline int32_t 
pgd_none(pgd_t pgd)
{
	return pgd_val(pgd) == 0;
}
static inline int32_t 
pgd_present(pgd_t pgd)
{
	return pgd_val(pgd) & _PAGE_PRESENT;
}
static inline int32_t 
pmd_none(pmd_t pmd)
{
	return pmd_val(pmd) == 0;
}
static inline int32_t 
pmd_present(pmd_t pmd)
{
	return pmd_val(pmd) & _PAGE_PRESENT;
}
static inline int32_t 
pte_none(pte_t pte)
{
	return pte_val(pte) == 0;
}
static inline int32_t 
pte_present(pte_t pte)
{
	return pte_val(pte) & _PAGE_PRESENT;
}
static inline int32_t 
pte_write(pte_t pte)
{
	return pte_val(pte) & _PAGE_RW;
}
static inline void 
pte_clearwrite(pte_t *pte)
{
	pte->pte_low &= ~_PAGE_RW;
}
static inline void 
pte_mkwrite(pte_t *pte)
{
	pte->pte_low |= _PAGE_RW;
}

static inline uint32_t 
pgd_index(viraddr_t address)
{
	return (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1));
}

static inline uint32_t 
pmd_index(viraddr_t address)
{
	return (((address) >> PMD_SHIFT) & (PTRS_PER_PMD-1));
}

static inline uint32_t 
pte_index(viraddr_t address)
{
	return (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE-1));
}

static inline viraddr_t 
pgd_page_vaddr(pgd_t pgd)
{
	return p2v(pgd_val(pgd) & PAGE_MASK);
}


static inline viraddr_t 
pmd_page_vaddr(pmd_t pmd)
{
	return p2v(pmd_val(pmd) & PAGE_MASK);
}

static inline viraddr_t 
pte_page_vaddr(pte_t pte)
{
	return p2v(pte_val(pte) & PAGE_MASK);
}

static inline pgd_t *
pgd_offset(pgd_t *pgd, viraddr_t address)
{
#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(address);
#endif
	return pgd + pgd_index(address);
}

static inline pmd_t *
pmd_offset(pgd_t *pgd, viraddr_t address) 
{
#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(address);
#endif

#ifdef CONFIG_PAE
    	return (pmd_t *)pgd_page_vaddr(*pgd) + pmd_index(address);
#else
	return (pmd_t *)pgd;
#endif
}

static inline pte_t *
pte_offset(pmd_t *pmd, viraddr_t address) 
{
#ifdef CONFIG_DEBUG
	assert(pmd);
	assert(address);
#endif
	return (pte_t *)pmd_page_vaddr(*pmd) + pte_index(address);
}


static inline physaddr_t 
_page2pfn(char *file,uint32_t line ,volatile struct page * page)
{
#ifdef CONFIG_DEBUG
	if(page < mempage || page >= mempage + max_pfn)
		_panic(file, line, "page2pfn called with invalid pfn 0x%08x", page - mempage);
#endif
	return page - mempage;
}
static inline physaddr_t 
_page2phys(char *file,uint32_t line ,volatile struct page * page)
{
#ifdef CONFIG_DEBUG
	if(page < mempage || page >= mempage + max_pfn)
		_panic(file, line, "page2phys called with invalid pfn 0x%08x", page - mempage);
#endif
	
	return _page2pfn(file,line ,page) << PAGE_SHIFT;
}
static inline viraddr_t 
_page2virt(char *file,uint32_t line ,volatile struct page * page)
{
#ifdef CONFIG_DEBUG
	if(page < mempage || page >= mempage + max_pfn)
		_panic(file, line, "page2virt called with invalid pfn 0x%08x", page - mempage);
#endif
	
	return p2v(_page2phys(file,line ,page));
}



static inline struct page *
_pfn2page(char *file,uint32_t line ,int pfn)
{
	if(pfn >= max_pfn)
		_panic(file, line, "pfn2page called with invalid pfn 0x%08x", pfn);
	return mempage + pfn;
}
static inline struct page *
_phys2page(char *file,uint32_t line ,viraddr_t phys)
{
	return _pfn2page(file,line ,PFN(phys));
}
static inline struct page *
_virt2page(char *file,uint32_t line ,viraddr_t vir)
{
	return _phys2page(file,line ,v2p(vir));
}

extern struct page* page_alloc(gfp_t);
extern struct page* pages_alloc(gfp_t,uint8_t);
extern void page_free(struct page* );
extern void pages_free(struct page*,uint8_t);
extern pte_t *page_walk(pgd_t *,viraddr_t ,bool );
extern int page_insert(pgd_t *,struct page *,viraddr_t , uint32_t );
extern void page_remove(pgd_t *, viraddr_t );
#endif/*!_MINIOS_PAGES_H_*/
