#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifdef DEBUG_0
#define USR_SZ_STACK 128
#define INITIAL_xPSR 0x10000000    /* user process initial xPSR value */
#endif /* DEBUG_0 */

typedef struct lnode_t{
	int address;
	struct lnode_t* next;
} lnode;

typedef struct list_t{
	int size;
	lnode* front;
	lnode* end;
} list;

typedef struct mmu_t{
	int * lookup_table;
	unsigned int free_mem;
	unsigned int max_mem;
	unsigned int lookup_table_size;
	list memory_in_use;
	list memory_not_in_use;
} MMU;

extern MMU* mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

void list_create_node_and_push(list *q, int a){
	lnode* curNode;
	curNode->address = a;
	curNode->next = NULL;

	q->end->next = curNode;
	q->end = curNode;
	q->size++;
}

void list_push(list *q, lnode *node){
	q->end->next = curNode;
	q->end = curNode;
	q->size++;
}

lnode* list_pop(list *q){
	lnode* curNode;
	curNode = q->front;

	q->front = q->front->next;

	q->size--;

	return curNode;
}

MMU* mmu_create_new_mmu(){
	int *temp, i;
	lnode* curNode;
	//initialize all vars
	MMU* memory_management_unit;
	memory_management_unit->free_mem = Image$$RW_IRAM1$$ZI$$Limit;
	memory_management_unit->max_mem = INITIAL_xPSR;

	curNode->address = INITIAL_xPSR;
	curNode->next = NULL;
	memory_management_unit->memory_not_in_use.front = curNode;
	memory_management_unit->memory_not_in_use.end = curNode;

	for (i; curNode->address != memory_management_unit->free_mem; curNode->address -= USR_SZ_STACK){
		list_create_node_and_push(&memory_management_unit->memory_not_in_use, curNode->address);
	}

	memory_management_unit->memory_in_use.front = NULL;
	memory_management_unit->memory_in_use.end = NULL;

	return memory_management_unit;
}

void mmu_init(){
	mmu = mmu_create_new_mmu();	
}

void * request_memory_block(){
	lnode* curNode;
	if (mmu->memory_not_in_use.size <= 0){
		return 0;
	}

	curNode = list_pop(&mmu->memory_not_in_use);
	list_push(&mmu->memory_in_use, curNode);

	return (void *)curNode->address;
}

int release_memory_block(void *MemoryBlock){
	lnode* curNode, prevNode;

	curNode = mmu->memory_in_use.front;

	//edge case where the node is at the beginning of the list
	if ((int)MemoryBlock == curNode->address){
		list_pop(&mmu->memory_in_use)
		list_push(&mmu->memory_not_in_use, curNode);
		return 0;
	}

	while (curNode!=mmu->memory_in_use.end){
		if ((int)MemoryBlock==curNode->address){
			//if found
			prevNode->next = curNode->next;	//remove the node out of cur list
			list_push(&mmu->memory_not_in_use, curNode);	//push it on to ther free one
			return 0;
		}
		prevNode = curNode;
		curNode = curNode->next;
	}
	
	return 1;

}
