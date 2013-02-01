#include "memory.h"
#include "process.h"

void mmu_init(){
	int i;
	mmu.free_mem = Image$$RW_IRAM1$$ZI$$Limit;
		//this is somewhere between 0x10000000 to 0x10008000

	mmu.max_mem = 0x10008000;
	mmu.lookup_table_size = 256;
	mmu.actual_size = (mmu.max_mem - mmu.free_mem) / USR_SZ_STACK;

	//set all flags in the lookup table to 0
	for (i = 0; i < mmu.lookup_table_size; i++){
		mmu.lookup_table[i] = 0;	//0 means free mem
	}
}

void* k_request_memory_block(){
	int i;
	while(1){
		for (i = 0; i < mmu.actual_size; i++){
			if (mmu.lookup_table[i]==0){
				//set the flag to in use
				mmu.lookup_table[i] = 1;
				//give out the mem address according to map
				return (void *)(mmu.max_mem - USR_SZ_STACK * i);
			}
		}
		//above loop loops through entire map for an empty spot
		//if I can't find it, I end up here then I release
		k_release_processor();
	}
	
}

int k_release_memory_block(void *MemoryBlock){
	int index;
	int mem_block_address = (int) MemoryBlock;
	//if the requested releasig address is outside of my range
	//return 1 (error)
	if (mem_block_address > mmu.max_mem || mem_block_address < mmu.free_mem)
		return 1; //you fucked up

	//calculates the index of the lookup table for that address and then set the flag to be freed
	index = (mmu.max_mem - mem_block_address) / USR_SZ_STACK;
	mmu.lookup_table[index] = 0;
	return 0;
}

