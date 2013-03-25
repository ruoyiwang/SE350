#include "memory.h"
#include "process.h"
#include "uart_polling.h"

void mmu_init(){
	int i, temp;
	mmu.free_mem = (unsigned int) &Image$$RW_IRAM1$$ZI$$Limit;
		//this is somewhere between 0x10000000 to 0x10008000

	//mmu.irq_using_mem1 = 0;
	//mmu.irq_using_mem2 = 0;
	//mmu.timer_using_mem = 0;
	//mmu.test_harness_using_mem = 0;
	//allocating/hardcoding 2 blocks to irqs
	//allocating/hardcoding 1 block to timer
	//allocating/hardcoding 1 block for test harness
	
	mmu.user_stack_table_size = 32;
	mmu.user_max_mem = 0x10008000 - 5 * USR_SZ_STACK;
	mmu.user_max_mem = 0x10008000;
	mmu.user_min_mem = mmu.user_max_mem - mmu.user_stack_table_size * USR_SZ_STACK;
	temp = (mmu.user_max_mem - mmu.user_min_mem) / USR_SZ_STACK;	//debug line
	//32 blocks for user
	for (i = 0; i < mmu.user_stack_table_size; i++){
		mmu.user_stack_table[i] = 0;	//0 means free mem
	}
	
	//below is for the kernal stack
	mmu.max_mem = 0x10008000 - mmu.user_stack_table_size*USR_SZ_STACK - USR_SZ_STACK;
	mmu.lookup_table_size = 64;
	mmu.actual_size = (mmu.max_mem - mmu.free_mem) / USR_SZ_STACK - 1;
//	mmu.actual_size = 64;
	mmu.memory_available = 1;	//set mem available to true

	//set all flags in the lookup table to 0
	for (i = 0; i < mmu.lookup_table_size; i++){
		mmu.lookup_table[i] = 0;	//0 means free mem
	}
}

void* k_request_memory_block(){
	int i;
	while(1){
		for (i = 0; i < mmu.user_stack_table_size; i++){
			if (mmu.user_stack_table[i]==0){
				//set the flag to in use
				mmu.user_stack_table[i] = 1;
				//give out the mem address according to map
				return (void *)((mmu.user_max_mem - USR_SZ_STACK * i) - USR_SZ_STACK + 4);
					//the - USR_SZ_STACK + 1 part would give the bottom of the stack instead of the top
					//usage of this pointer would be ++ instead of --
					//not shortening the formula because it's not clear other wise
			}
		}
		//above loop loops through entire map for an empty spot
		//if I can't find it, I end up here then I release
		mmu.memory_available = 0;	//no more mem available
		block_current_process();	//block the current process
		k_release_processor();
	}
}

void* k_request_kernel_memory_block(){
	int i;
	while(1){
		for (i = 0; i < mmu.actual_size; i++){
			if (mmu.lookup_table[i]==0){
				//set the flag to in use
				mmu.lookup_table[i] = 1;
				//give out the mem address according to map
				return (void *)((mmu.max_mem - USR_SZ_STACK * i) - USR_SZ_STACK + 4);
					//the - USR_SZ_STACK + 1 part would give the bottom of the stack instead of the top
					//usage of this pointer would be ++ instead of --
					//not shortening the formula because it's not clear other wise
			}
		}
		/*
		//above loop loops through entire map for an empty spot
		//if I can't find it, I end up here then I release
		mmu.memory_available = 0;	//no more mem available
		block_current_process();	//block the current process
		k_release_processor();
		*/
	}
}

/*
void* k_request_irq_memory_block(){
	if (mmu.irq_using_mem1 == 0){
		mmu.irq_using_mem1 = 1;
		return (void *)(0x10008000 - USR_SZ_STACK + 4);
			//returning the bottom of the stack
	}
	else if (mmu.irq_using_mem2 == 0){
		mmu.irq_using_mem2 = 1;
		return (void *)(0x10008000 - 2 * USR_SZ_STACK + 4);
			//returning the bottom of the stack
	}
	else {
		return NULL;
			//this would suck
	}
}

void* k_request_timer_memory_block(){
	//if (mmu.timer_using_mem == 0){
		//mmu.timer_using_mem = 1;
		return (void *)(0x10008000 - 3 * USR_SZ_STACK + 4);
	//}
	//else {
		//return NULL;
			//fml
	//}
}

void* k_request_test_harness_memory_block(){
	if (mmu.test_harness_using_mem == 0){
		mmu.test_harness_using_mem = 1;
		return (void *)(0x10008000 - 4 * USR_SZ_STACK + 4);
	}
	else {
		return NULL;
			//fml
	}
}
*/
int k_release_memory_block(void *MemoryBlock){
	int index;
	int temp;
	int mem_block_address = (int) MemoryBlock;
	//if the requested releasig address is outside of my range
	//return 1 (error)
	if (mem_block_address > 0x10008000 || mem_block_address < mmu.free_mem){	
		return 1;
	}
/*
	if (mem_block_address <= 0x10008000 && mem_block_address > 0x10008000 - USR_SZ_STACK){
		//means this is irq mem 1
		mmu.irq_using_mem1 = 0;
		return 0;
	}
	else if (mem_block_address <= 0x10008000 - USR_SZ_STACK && mem_block_address > 0x10008000 - 2 * USR_SZ_STACK){
		//means this is irq mem 2
		mmu.irq_using_mem2 = 0;
		return 0;
	}
	else if (mem_block_address <= 0x10008000 - 2 * USR_SZ_STACK && mem_block_address > 0x10008000 - 3 * USR_SZ_STACK){
		//means this is timer mem
		mmu.timer_using_mem = 0;
		return 0;
	}
	else if (mem_block_address <= 0x10008000 - 3 * USR_SZ_STACK && mem_block_address > 0x10008000 - 4 * USR_SZ_STACK){
		//means this is test harness mem
		mmu.test_harness_using_mem = 0;
		return 0;
	}
*/
	//THIS IS RELEASING USER MEMORY
	for (index = 0; index < mmu.user_stack_table_size; index++){
		if ((mmu.user_max_mem - USR_SZ_STACK * index >= mem_block_address) && ((mmu.user_max_mem - USR_SZ_STACK * index) - USR_SZ_STACK + 4) <= mem_block_address){
			//if the memory given too me is within this range, then the index is correct
			
			temp = mmu.user_stack_table[index];		//the soul purpose of this line is so we can debug 
																		//by looking at if they are releasing a used block
			mmu.user_stack_table[index] = 0;
			if (mmu.memory_available == 0){
				mmu.memory_available = 1;		//when we release mem, there's mem avaialble again
				k_release_processor();		
			}
			return 0;
		}
	}
	
	//calculates the index of the lookup table for that address and then set the flag to be freed
	//THIS IS RELEASING KERNAL MEMORY
	for (index = 0; index < mmu.actual_size; index++){
		if ((mmu.max_mem - USR_SZ_STACK * index >= mem_block_address) && ((mmu.max_mem - USR_SZ_STACK * index) - USR_SZ_STACK + 4) <= mem_block_address){
			//if the memory given too me is within this range, then the index is correct
			temp = mmu.lookup_table[index];		//the soul purpose of this line is so we can debug 
																		//by looking at if they are releasing a used block
			mmu.lookup_table[index] = 0;
			
			return 0;
		}
	}

	//this is obv wrong if it gets here
	return 1;

}

