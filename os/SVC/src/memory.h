#ifndef NULL
#define NULL ((void *)0) 
#endif

typedef struct{
	bool * lookup_table;
	unsigned int free_mem;
	unsigned int max_mem;
	unsigned int lookup_table_size;
	const unsigned int block_size = 120;
} MMU;


MMU* mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

MMU*  mmu_create_new_mmu(){
	int *temp;
	//initialize all vars
	MMU* memory_management_unit;
	memory_management_unit->free_mem = Image$$RW_IRAM1$$ZI$$Limit;
	memory_management_unit->max_mem = 0x10000000;
	memory_management_unit->lookup_table = NULL;

	//loop to init the lookup table
	memory_management_unit->lookup_table_size = (max_mem - free_mem) / 4;
	// allocate blocks of size 120
	*temp = realloc(lookup_table, memory_management_unit->lookup_table_size*sizeof(bool));
	if ( temp != NULL ) //realloc was successful
	{
		lookup_table = temp;
	}
	else{
		return NULL;
	}
	for (int i = 0; i < memory_management_unit->lookup_table_size; i++){
		lookup_table[i] = 0;	//set all bits to not used
	}
	return memory_management_unit;
}

mmu = mmu_create_new_mmu();

void * request_memory_block(){
	for (int i = 0; i < mmu->lookup_table_size; i+=mmu->lookup_table_size){
		if (mmu->lookup_table[i] == 0)｛
			for (int j = i; j < i + mmu->lookup_table_size; j++){
				mmu->lookup_table[j] = 1;	//assign all of the 120 blocks to 1
			}
			return i*4 + mmu->free_mem;	//return the actual address
		｝
	}
	return NULL;
}

int release_memory_block(void *MemoryBlock){
	unsigned int p = (MemoryBlock - mmu->free_mem) / 4;
	if(MemoryBlock > 0x10000000)
		return -1;
	for (int i = 0; i < mmu->block_size; i++, p++){
		mmu->lookup_table[p] = 0;
	}
	return 1;
}