#ifndef _3LEVEL_H_
#define _3LEVEL_H_ 

#include<types.h>
#define PAGE_SHIFT		(12)
#define PMD_SHIFT		(21)
#define PGDIR_SHIFT		(30)

#define PAGE_SIZE       	(1UL << PAGE_SHIFT)
#define PMD_SIZE        	(1UL << PMD_SHIFT)
#define PGDIR_SIZE		(1UL << PGDIR_SHIFT)

#define PAGE_MASK		(~(PAGE_SIZE-1ULL))
#define PMD_MASK		(~(PMD_SIZE-1ULL))
#define PGDIR_MASK		(~(PGDIR_SIZE-1ULL))

#define PTRS_PER_PTE		(512)
#define PTRS_PER_PMD		(512)
#define PTRS_PER_PGD		(4)


typedef struct { uint64_t pte_low; }pte_t;
typedef struct { uint64_t pmd; }pmd_t;
typedef struct { uint64_t pgd; }pgd_t;
#endif/*!_3LEVEL_H_*/
