#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifdef DEBUG_0
#define USR_SZ_STACK 128
#define INITIAL_xPSR 0x10000000    /* user process initial xPSR value */
#endif /* DEBUG_0 */

typedef unsigned int U32;

#define __SVC_0  __svc_indirect(0)

typedef struct mmu_t{
	char lookup_table[256];
	unsigned int free_mem;
	unsigned int max_mem;
	unsigned int lookup_table_size;
	unsigned int actual_size;
} MMU;

extern MMU mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

extern void* request_memory_block(void);
#define k_request_memory_block() _request_memory_block((U32)request_memory_block)
extern void* _request_memory_block(U32 p_func) __SVC_0;
//void* request_memory_block(void) __svc_indirect(0);

extern int release_memory_block(void *);
#define k_release_memory_block(MemoryBlock) _release_memory_block((U32)release_memory_block, MemoryBlock)
extern int _release_memory_block(U32 p_func, void *MemoryBlock) __SVC_0;
//int __svc_indirect(0) release_memory_block(void *MemoryBlock);

void mmu_init(){
	int i;
	mmu.free_mem = Image$$RW_IRAM1$$ZI$$Limit;
	mmu.max_mem = 0x10008000;
	mmu.lookup_table_size = 256;
	mmu.actual_size = (mmu.max_mem - mmu.free_mem) / 128;
	for (i = 0; i < mmu.lookup_table_size; i++){
		mmu.lookup_table[i] = 0;	//0 means free mem
	}
}

void* request_memory_block(){
	int i;
	for (i = 0; i < mmu.actual_size; i++){
		if (mmu.lookup_table[i]==0){
			mmu.lookup_table[i] = 1;
			return (void *)(mmu.max_mem - USR_SZ_STACK * i);
		}
	}
}

int release_memory_block(void *MemoryBlock){
	int i, index;
	int mem_block_address = (int) MemoryBlock;
	if (mem_block_address > mmu.max_mem || mem_block_address < mmu.free_mem)
		return 1; //you fucked up

	index = (mmu.max_mem - mem_block_address) / 128;
	mmu.lookup_table[index] = 0;
}
