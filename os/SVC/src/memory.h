#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifndef __MEMORY__
#define __MEMORY__

#define USR_SZ_STACK 128
#define INITIAL_xPSR 0x01000000    /* user process initial xPSR value */

typedef unsigned int U32;

#define __SVC_0  __svc_indirect(0)

typedef struct mmu_t{
	char lookup_table[256];
	unsigned int free_mem;
	unsigned int max_mem;
	char irq_using_mem1, irq_using_mem2, timer_using_mem;
	unsigned int lookup_table_size;
	unsigned int actual_size;
	char memory_available;	//1 is there is avail mem, 0 is there isn't
} MMU;

extern MMU mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

void mmu_init(void);

extern void* k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void* _request_memory_block(U32 p_func) __SVC_0;
//void* request_memory_block(void) __svc_indirect(0);

extern int k_release_memory_block(void *);
#define release_memory_block(MemoryBlock) _release_memory_block((U32)k_release_memory_block, MemoryBlock)
extern int _release_memory_block(U32 p_func, void *MemoryBlock) __SVC_0;
//int __svc_indirect(0) release_memory_block(void *MemoryBlock);

extern void* k_request_irq_memory_block(void);
#define request_irq_memory_block() _request_irq_memory_block((U32)k_request_irq_memory_block)
extern void* _request_irq_memory_block(U32 p_func) __SVC_0;

extern void* k_request_timer_memory_block(void);
#define request_timer_memory_block() _request_timer_memory_block((U32)k_request_timer_memory_block)
extern void* _request_timer_memory_block(U32 p_func) __SVC_0;
//lookup_table - actual address map
//    0    0x10008000
//    1    0x10008000 - 128 * 1
//    2    0x10008000 - 128 * 2
//    3    0x10008000 - 128 * 3
//   ...   ..........
//    i    0x10008000 - 128 * i

#endif
	//__MEMORY__
