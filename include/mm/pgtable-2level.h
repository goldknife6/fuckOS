#ifndef _2LEVEL_H_
#define _2LEVEL_H_ 

#include<types.h>
#define PAGE_SHIFT		(12)
#define PMD_SHIFT		(22) //non-PAE
#define PGDIR_SHIFT		(22)

#define PAGE_SIZE       	(1UL << PAGE_SHIFT)
#define PMD_SIZE        	(1UL << PMD_SHIFT)
#define PGDIR_SIZE		(1UL << PGDIR_SHIFT)

#define PAGE_MASK		(~(PAGE_SIZE-1))
#define PMD_MASK		(~(PMD_SIZE-1))
#define PGDIR_MASK		(~(PGDIR_SIZE-1))

#define PTRS_PER_PTE		(1024)
#define PTRS_PER_PMD		(1)
#define PTRS_PER_PGD		(1024)


typedef struct { uint32_t pte_low; }pte_t;
typedef struct { uint32_t pmd; }pmd_t;
typedef struct { uint32_t pgd; }pgd_t;
#endif/**/
