#include <types.h>
#include <string.h>

#include <fuckOS/kernel.h>
#include <fuckOS/assert.h>

#include <mm/pages.h>
#include <mm/layout.h>

#include <asm/x86.h>
struct mmap
{
	uint64_t	addr;
	uint64_t	len;
	uint32_t	type;
	uint32_t 	zero;
};

extern void detect_memory(void*);
extern char _BSS_END[];

static void printmapp();
static void get_max_pfn();
static void get_normal_maxpfn();
static void get_high_maxpfn();
static void get_totalram_pages();
static void bootmm_init();

struct mmap mmapinfo[128];
int nr_mmapinfo;
int max_pfn;
int normal_maxpfn;
int high_maxpfn;
int totalram_pages;
pgd_t *kpgd;

struct page *mempage;
/*
*	获取内存分布信息
*	计算各种全局变量的值
*/
void bootmm(void* ginfo)
{

	detect_memory(ginfo);

	printmapp();

	get_max_pfn();

	get_normal_maxpfn();

	get_high_maxpfn();

	get_totalram_pages();

	bootmm_init();
}


/*
*	获取最大可用页号
*	mmapinfo是从detect_memory计算出来的
*/
static void get_totalram_pages()
{
	struct mmap *mmap = mmapinfo;
	struct mmap *end = mmapinfo + nr_mmapinfo;

	uint32_t last = 0;

	for(mmap ;mmap < end;mmap++) {
		if(mmap->type == 1) {
			last += (mmap->len / PAGE_SIZE);
		}
		
	}
	totalram_pages = last;
}

static void get_max_pfn()
{
	struct mmap *mmap = mmapinfo;
	struct mmap *end = mmapinfo + nr_mmapinfo;

	uint64_t last;

	for(mmap ;mmap < end;mmap++) {
		if(mmap->type == 1) {
			last = mmap->addr + mmap->len;
		}
	}
	max_pfn = PFN(last);
#ifndef	CONFIG_PAE
	if(max_pfn >= 0x100000) {
		max_pfn = 0x100000;
	}
#endif
	printk("max_pfn %x ",max_pfn);
}


static void get_normal_maxpfn()
{
	struct mmap *mmap = mmapinfo;
	struct mmap *end = mmapinfo + nr_mmapinfo;

	int last = 0;

	for(mmap ;mmap < end;mmap++) {
		if(mmap->type == 1) {
			if (mmap->addr >= KERNEL_NORMAL - KERNEL_BASE_ADDR)
				continue;

			if (mmap->addr + mmap->len < KERNEL_NORMAL - KERNEL_BASE_ADDR) {
				if (mmap->addr + mmap->len > last)
					last = mmap->addr + mmap->len;
			} else{
				last = PFN(KERNEL_NORMAL - KERNEL_BASE_ADDR);
				break;
			}
		}
	}
	normal_maxpfn = last;
#ifdef	CONFIG_DEBUG
	printk("normal_maxpfn:%x ",last);
#endif
}

static void get_high_maxpfn()
{
	struct mmap *mmap = mmapinfo;
	struct mmap *end = mmapinfo + nr_mmapinfo;

	uint64_t last = 0;

	for(mmap ;mmap < end;mmap++) {
		if(mmap->type == 1) {
			if (mmap->addr + mmap->len <= KERNEL_NORMAL - KERNEL_BASE_ADDR)
				continue;
			
			if (mmap->addr + mmap->len > last)
					last = mmap->addr + mmap->len;
		
		}
	}
	high_maxpfn = (last & ~0xFFF) >> PAGE_SHIFT;
#ifdef	CONFIG_DEBUG
	printk("high_maxpfn:%x ",high_maxpfn);
#endif
}

/*
*	
*/
static void *alloc_bootmm_pages(int size)
{
	static	void *base = NULL;
	if (!base) {
		base = PAGE_ALIGN(_BSS_END + 0xFFF);
	}
#ifdef	CONFIG_DEBUG
	assert(size > 0);
	printk("base:%x\n",base);
#endif	
	void *tmp = base;
	base += size * PAGE_SIZE;
	return tmp;
}

static int is_page_aval(int pfn)
{
	return 0;
}
/*
struct page {
	//struct list_head list;
	//atomic_t	nref;
	uint32_t	flags;
	int32_t		order;

	//struct kmem_cache* slab;
	void ** 	freelist;//slab
	int32_t		inuse;
	//spinlock_t	slublock;
};
*/
static void mempage_init()
{
	int i;
	

	for(i = 0 ;i < max_pfn;i++) {
		if(is_page_aval(i)) {
			memset(&mempage[i],0,sizeof(struct page));
			mempage[i].flags = _pg_free;
			INIT_LIST_HEAD(&mempage[i].list);
		}
	}
}

void boot_map_region(pgd_t *pgdir, viraddr_t va, 
			size_t size, physaddr_t pa, int perm)
{
	uint32_t i;
	for ( i = 0; i < size; i += PAGE_SIZE ) {
		pte_t* pte = NULL;
		//pte = page_walk(pgdir, va + i, true);
		assert(pte);
		pte_set(pte, pa, perm | _PAGE_PRESENT);
	}
	
}
extern void page_check();

static void bootmm_init()
{
	int num;
	pgd_t *kpgd = alloc_bootmm_pages(1);

	page_check();

	//映射内核页目录
	boot_map_region(kpgd, KERNEL_BASE_ADDR, 
				KERNEL_NORMAL - KERNEL_BASE_ADDR, 0, _PAGE_RW);

	
	lcr3(pte2p(kpgd));

	
	num = max_pfn * sizeof(struct page) / PAGE_SIZE + 1;
	mempage = alloc_bootmm_pages(num);
	memset(mempage,0xFF,num * PAGE_SIZE);
	mempage_init();
}

static void printmapp()
{
	struct mmap *mmap = mmapinfo;
	struct mmap *end = mmapinfo + nr_mmapinfo;
	for(mmap ;mmap < end;mmap++) {
		printk ("0x%016llx 0x%016llx %x\n",mmap->addr,mmap->len,mmap->type);
	}
}
