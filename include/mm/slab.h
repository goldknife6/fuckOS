#ifndef _SLAB_H_
#define _SLAB_H_ 
#include <types.h>

#include <fuckOS/list.h>
#include <fuckOS/cpu.h>

#include <mm/pages.h>

#include <asm/atomic.h>

#define	SLABORDER	10

struct mem_cache_node {
	spinlock_t	list_lock;
	struct list_head partial;		//Partial slab 的双向循环队列
	struct list_head full;
	int32_t		nfull;
	int32_t		npartial;
	int32_t		nslub;
};

struct mem_cache_cpu {
	void **	freelist;
	struct page *page;
};

struct mem_cache {
	int32_t size;				//分配给对象的内存大小（可能大于对象的实际大小）
	int32_t objsize;			//对象的实际大小
	int32_t offset;			//存放空闲对象指针的位移
	int32_t order;				//表示一个 slab 需要 2^order 个物理页框
	int32_t objects;			//一个 slab 中的对象总个数
	void (*ctor)(void);			//创建 slab 时用于初始化每个对象的构造函数
	struct mem_cache_node node;			//为每个节点创建的 slab 信息的数据结构
	struct mem_cache_cpu cpu_slab[CPUNUMS];
	
	struct list_head list;
};


extern void slab_init();
extern void get_slab_info();
extern void* kmalloc(size_t size);
extern void kfree(void* );
#endif/*!_SLAB_H_ */
