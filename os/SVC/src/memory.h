#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifdef DEBUG_0
#define USR_SZ_STACK 0x120
#endif /* DEBUG_0 */


typedef struct mmu_t{
	int * lookup_table;
	unsigned int free_mem;
	unsigned int max_mem;
	unsigned int lookup_table_size;
} MMU;

MMU* mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

MMU* mmu_create_new_mmu(){
	int *temp, i;
	//initialize all vars
	MMU* memory_management_unit;
	memory_management_unit->free_mem = Image$$RW_IRAM1$$ZI$$Limit;
	memory_management_unit->max_mem = 0x10000000;
	memory_management_unit->lookup_table = NULL;

	//loop to init the lookup table
	memory_management_unit->lookup_table_size = (memory_management_unit->max_mem - memory_management_unit->free_mem) / USR_SZ_STACK;
	
	// allocate blocks of size 120
	*temp = realloc(memory_management_unit->lookup_table, memory_management_unit->lookup_table_size*sizeof(int));
	if ( temp != NULL ) //realloc was successful
	{
		memory_management_unit->lookup_table = temp;
	}
	else{
		return NULL;	//it fucked up
	}
	
	for (i = 0; i < memory_management_unit->lookup_table_size; i++){
		memory_management_unit->lookup_table[i] = 0;	//set all blocks to be free
	}
	return memory_management_unit;
}

void mmu_init(){
	mmu = mmu_create_new_mmu();	
}

void * request_memory_block(){
	int i;
	for (i = 0; i < mmu->lookup_table_size; i+=mmu->lookup_table_size){
		if (mmu->lookup_table[i] == 0){
			return (void *)(i*USR_SZ_STACK + mmu->free_mem);	//return the actual address
		}
	}
	return NULL;
}

int release_memory_block(void *MemoryBlock){
	unsigned int p = ((int)MemoryBlock - mmu->free_mem) / 4;
	if((int)MemoryBlock > 0x10000000)
		return 1;	//FAIL
	
	mmu->lookup_table[p] = 0;
	return 0;		//true
}
