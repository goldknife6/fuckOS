#include <string.h>
#include <errno.h>

#include <mm/pages.h>
#include <mm/mmu.h>
#include <mm/mmzone.h>

#include <fuckOS/kernel.h>

#include <asm/x86.h>

void page_check()
{

}

static struct page* 
_pages_alloc(struct zone_struct *zone ,
			gfp_t flage,uint32_t order)
{
	struct page* page = NULL;

	if ((flage & _GFP_ZERO) && (flage & _GFP_HIGH))
		panic("(flage & _GFP_ZERO) && (flage & _GFP_HIGH)\n");

	page = alloc_buddy(zone,order);

	if(page == NULL)
		goto out;

	if (flage & _GFP_ZERO) {
		int i = PAGE_SIZE << order;
		memset((void*)page2virt(page),0,i);
	}

out:
	return page;
}
void pages_free(struct page* page,uint8_t order)
{
#ifdef CONFIG_DEBUG
	assert(page);
	assert(order >= 0);
	assert(order <= MAXORDER);
#endif	
	if(page2phys(page) < NORMAL_ADDR)
		free_buddy(&zone_normal,page,order);
	else
		free_buddy(&zone_high,page,order);
}

void page_free(struct page* page)
{
	pages_free(page,0);
}


struct page* 
page_alloc(gfp_t flage)
{
	return pages_alloc(flage,0);
}


struct page* 
pages_alloc(gfp_t flage,uint8_t order)
{
	struct page* page = NULL;
#ifdef CONFIG_DEBUG
	assert(order >= 0);
	assert(order <= MAXORDER);
#endif
	if((flage & _GFP_ZERO) && (flage & _GFP_HIGH))
		goto out;
	if(flage & _GFP_HIGH)
		page = _pages_alloc(&zone_high ,flage,order);
	else
		page = _pages_alloc(&zone_normal ,flage,order);
out:
	return page;
}

void refresh_tlb(pgd_t *pgd, viraddr_t va)
{
	// Flush the entry only if we're modifying the current address space.
	//if (!curenv || curenv->env_pgdir == pgdir)
	invlpg((void*)va);
	int cr3 = rcr3();
	lcr3(cr3);
}

void page_decref(struct page* page)
{
	if (atomic_dec_and_test(&page->nref)) {
		page_free(page);
	}
}

struct page* 
page_lookup(pgd_t *pgd, 
		viraddr_t va, pte_t **pte_store)
{
#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(va);
#endif
	pte_t* pte = page_walk(pgd, va, false);
	if( pte == NULL ) 
		return NULL;
	if(!pte_present(*pte)) {
		//printk("what?:%x %llx\n",va,*pte);
		return NULL;
	}
	if ( NULL != pte_store )
		*pte_store = pte;
	return virt2page(pte_page_vaddr(*pte));
}

void page_remove(pgd_t *pgd, viraddr_t va) 
{
	struct page *page;
	pte_t *pte_store;

#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(va);
#endif
	page = page_lookup(pgd, va, &pte_store);

	if( page == NULL ) return;
	
	page_decref(page);

	pte_set(pte_store,0,0);

	refresh_tlb(pgd, va);
}

int page_insert(pgd_t *pgd, struct page *page,viraddr_t va, uint32_t perm)
{
#ifdef CONFIG_DEBUG
	assert(pgd);
	assert(page);
	assert(va);
#endif
	pte_t* pte = page_walk(pgd, va,true);

	if (!pte) return -ENOMEM;

	atomic_inc(&page->nref);

	if (pte_present(*pte)) 
		page_remove(pgd, va);	
	
	pte_set(pte, page2phys(page) , perm);

	refresh_tlb(pgd, va);
	return 0;
}

static int get_pmd(pgd_t *pgd,int perm)
{
	struct page* page;
	physaddr_t pa;
	page = pages_alloc(_GFP_ZERO,0);
	if (!page)
		return -1;
	atomic_inc(&page->nref);
	pa = page2phys(page);
#ifdef CONFIG_PAE
	uint32_t pe = perm & ~(_PAGE_RW | _PAGE_USER);
#else
	uint32_t pe = perm;
#endif
	pgd_set(pgd,pa,pe);
	return 0;
}

static int get_pte(pmd_t *pmd,int perm)
{
	struct page* page;
	physaddr_t pa;
	page = pages_alloc(_GFP_ZERO,0);
	if (!page)
		return -1;
	atomic_inc(&page->nref);
	pa = page2phys(page);
	pmd_set(pmd,pa,perm);
	return 0;
}

/*
*page_walk - look up a pte_t from a user-virtual address
*/
pte_t *page_walk(pgd_t *pgdp,
			viraddr_t address,bool create)
{
#ifdef CONFIG_DEBUG
	//assert(pgdp);
	//assert(address);
#endif

	pgd_t * pgd = NULL;
	pte_t * pte = NULL;
	pmd_t * pmd = NULL;
	struct page *page = NULL;


	pgd =  pgd_offset(pgdp, address);

	if (pgd_none(*pgd) && !create)
		return NULL;
	else if (pgd_none(*pgd)) {
		if( get_pmd(pgd,_PAGE_PRESENT | _PAGE_RW | _PAGE_USER) < 0)
			return NULL;
		pgd =  pgd_offset(pgdp, address);
	}
	pmd =  pmd_offset(pgd, address);
	if (pmd_none(*pmd) && !create)
		return NULL;
	else if (pmd_none(*pmd)) {
		if( get_pte(pmd,_PAGE_PRESENT | _PAGE_RW | _PAGE_USER) < 0)
			return NULL;
		pmd =  pmd_offset (pgd, address);
	}
	
	pte = pte_offset(pmd, address);
	return pte;
}


