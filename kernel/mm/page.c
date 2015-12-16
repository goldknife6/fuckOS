#include <string.h>
#include <errno.h>

#include <mm/pages.h>
#include <mm/mmu.h>


#include <fuckOS/kernel.h>

#include <asm/x86.h>

void page_check()
{
	struct page *page = mempage;
	pte_t pte;
	int pfn;
	viraddr_t viraddr;
	physaddr_t physaddr;

	printk("PAGE_MASK:%"PLEN"x PMD_MASK:%"PLEN"x PGDIR_MASK:%"PLEN"x\n",PAGE_MASK,PMD_MASK,PGDIR_MASK);

	pte_set(&pte,0x1100000,0x212);
	printk("pte:%"PLEN"x\n",pte_val(pte));

	viraddr = 0xC0000000;
	physaddr = v2p(viraddr);
	printk("physaddr:%"PLEN"x\n",physaddr);

	physaddr = 0x30000000-1;
	viraddr = p2v(physaddr);
	printk("viraddr:%08x\n",viraddr);

	pfn = page2pfn((struct page *)((char*)page + 100));
	printk("pfn:%08x\n",pfn);

	physaddr = page2phys(page);
	printk("physaddr:%"PLEN"x\n",physaddr);

	viraddr = page2virt(page+0x30000-1);
	printk("viraddr:%"PLEN"x\n",viraddr);

	int num = 0x10000;
	page = pfn2page(num);
	if(num != page2pfn(page))
		panic("page2pfn\n");

	physaddr = 0x00001100;
	page = phys2page(physaddr);
	physaddr = page2phys(page);
	printk("physaddr:%"PLEN"x\n",physaddr);

	viraddr = 0x12345678;

	printk("address:%x pgd:%x pmd:%x pte:%x\n",viraddr,
		pgd_index(viraddr),pmd_index(viraddr),pte_index(viraddr));

	viraddr = pte_page_vaddr(pte);
	printk("viraddr:%x\n",viraddr);
}
struct page* pages_alloc(gfp_t flage,int8_t order)
{
	struct page *page = NULL;
/*
	if((flage & _GFP_ZERO) && (flage & _GFP_HIGH))
		goto out;
	if(flage & _GFP_HIGH)
		page = __pages_alloc (&zone_high ,flage,order);
	else
		page = __pages_alloc (&zone_normal ,flage,order);
out:
*/
	return page;
}


static int get_pmd(pgd_t *pgd,int perm)
{
	struct page* page;
	physaddr_t pa;
	page = pages_alloc(_GFP_ZERO,0);
	pa = page2phys(page);
	pgd_set(pgd,pa,perm);
	return 0;
}

static int get_pte(pmd_t *pmd,int perm)
{
	struct page* page;
	physaddr_t pa;
	page = pages_alloc(_GFP_ZERO,0);
	pa = page2phys(page);
	pmd_set(pmd,pa,perm);
	return 0;
}

/*
*page_walk - look up a pte_t from a user-virtual address
*/
pte_t *page_walk (struct mm_struct* mm,viraddr_t address,bool create)
{
#ifdef CONFIG_DEBUG
	assert(mm);
	assert(address);
#endif

	pgd_t * pgd = NULL;
	pte_t * pte = NULL;
	pmd_t * pmd = NULL;
	struct page *page = NULL;

	spin_lock(&mm->page_table_lock);

	pgd =  pgd_offset (mm->mm_pgd, address);

	if (pgd_none(*pgd) && !create)
		goto unlock;
	else if (pgd_none(*pgd)) {
		if( get_pmd(pgd,_PAGE_PRESENT | _PAGE_RW | _PAGE_USER) < 0)
			goto unlock;
		pgd =  pgd_offset (mm->mm_pgd, address);
	}
	pmd =  pmd_offset (pgd, address);
	if (pmd_none(*pmd) && !create)
		goto unlock;
	else if (pmd_none(*pmd)) {
		if( get_pte(pmd,_PAGE_PRESENT | _PAGE_RW | _PAGE_USER) < 0)
			goto unlock;
		pmd =  pmd_offset (pgd, address);
	}
	pte = pte_offset (pmd, address);
unlock:
	spin_unlock(&mm->page_table_lock);
	return pte;
}
