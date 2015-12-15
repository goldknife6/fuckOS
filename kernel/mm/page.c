#include <string.h>
#include <errno.h>

#include <mm/pages.h>
#include <mm/mmu.h>
#include <mm/layout.h>

#include <fuckOS/kernel.h>

#include <asm/x86.h>


void set_pgde(pgd_t * pgde,struct page* pf,viraddr_t address,uint32_t perm)
{
#ifdef CONFIG_DEBUG
	assert(pgde);
	assert(pf);
	assert(page2pfn(pf) || !page2pfn(pf));
#endif	

	uint32_t pgd_num = pgd_index(address);
#ifdef CONFIG_PAE
	uint32_t pe = perm & ~(_PAGE_RW | _PAGE_USER);
#else
	uint32_t pe = perm;
#endif
}

void set_pmde(pgd_t * pgde,struct page* pf,viraddr_t address,uint32_t perm)
{
#ifdef CONFIG_DEBUG
	assert(pgde);
	assert(pf);
	assert(page2pfn(pf) || !page2pfn(pf));
#endif	
	pmd_t * pmde = NULL;
	pmde = (pmd_t *)pgd_page_vaddr(*pgde) + pmd_index(address);
}

pte_t *page_walk (pgd_t *pgd,viraddr_t address,bool create)
{
#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(address);
#endif

	return NULL;
}
