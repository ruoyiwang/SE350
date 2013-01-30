#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifdef DEBUG_0
#define USR_SZ_STACK 128
#define INITIAL_xPSR 0x10000000    /* user process initial xPSR value */
#endif /* DEBUG_0 */

typedef unsigned int U32;

#define __SVC_0  __svc_indirect(0)

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
	list *memory_in_use;
	list *memory_not_in_use;
} MMU;

extern MMU* mmu;	//declarations

extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;

extern void list_push(list *, lnode*);
#define k_list_push(q, node) _list_push((U32)list_push, q, node)
extern void _list_push(U32 p_func, list *q, lnode *node) __SVC_0;
//void  __svc_indirect(0) list_push(list *q, lnode *node);

extern lnode* list_pop(list*);
#define k_list_pop(q) _list_pop((U32)list_pop, q)
extern lnode* _list_pop(U32 p_func, list *q) __SVC_0;
//lnode* list_pop(list *q) __svc_indirect(0);

extern void list_create_node_and_push(list*, int);
#define k_list_create_node_and_push(q, a) _list_create_node_and_push((U32)list_create_node_and_push, q, a)
extern void _list_create_node_and_push(U32 p_func, list *q, int a) __SVC_0;
//void  __svc_indirect(0) list_create_node_and_push(list *q, int a);

extern MMU* mmu_create_new_mmu(void);
#define k_mmu_create_new_mmu() _mmu_create_new_mmu((U32)mmu_create_new_mmu)
extern MMU* _mmu_create_new_mmu(U32 p_func) __SVC_0;
//MMU* mmu_create_new_mmu(void) __svc_indirect(0);


extern void* request_memory_block(void);
#define k_request_memory_block() _request_memory_block((U32)request_memory_block)
extern void* _request_memory_block(U32 p_func) __SVC_0;
//void* request_memory_block(void) __svc_indirect(0);

extern int release_memory_block(void *);
#define k_release_memory_block(MemoryBlock) _release_memory_block((U32)release_memory_block, MemoryBlock)
extern int _release_memory_block(U32 p_func, void *MemoryBlock) __SVC_0;
//int __svc_indirect(0) release_memory_block(void *MemoryBlock);

void  list_create_node_and_push(list *q, int a){
	lnode* curNode;
	curNode->address = a;
	curNode->next = NULL;

	q->end->next = curNode;
	q->end = curNode;
	q->size++;
}

void  list_push(list *q, lnode *node){
	q->end->next = node;
	q->end = node;
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
	volatile lnode* curNode;
	volatile int address = INITIAL_xPSR;
	//initialize all vars
	volatile MMU* memory_management_unit;
	volatile list* free_memory,* not_free_memory;
	
	free_memory->front = curNode;
	free_memory->end = curNode;
	memory_management_unit->memory_not_in_use = free_memory;
	memory_management_unit->memory_in_use = not_free_memory;
	
	memory_management_unit->free_mem = Image$$RW_IRAM1$$ZI$$Limit;
	memory_management_unit->max_mem = INITIAL_xPSR;

	curNode->address = INITIAL_xPSR;
	curNode->next = NULL;
//	memory_management_unit->memory_not_in_use->front = curNode;
//	memory_management_unit->memory_not_in_use->end = curNode;
	for (; address != memory_management_unit->free_mem; address -= USR_SZ_STACK){
		list_create_node_and_push(memory_management_unit->memory_not_in_use, address);
	}

	memory_management_unit->memory_in_use->front = NULL;
	memory_management_unit->memory_in_use->end = NULL;

	return memory_management_unit;
}

void mmu_init(){
	mmu = mmu_create_new_mmu();	
}

void* request_memory_block(){
	lnode* curNode;
	if (mmu->memory_not_in_use->size <= 0){
		return 0;
	}

	curNode = list_pop(mmu->memory_not_in_use);
	list_push(mmu->memory_in_use, curNode);

	return (void *)curNode->address;
}

int release_memory_block(void *MemoryBlock){
	lnode* curNode, *prevNode;

	curNode = mmu->memory_in_use->front;

	//edge case where the node is at the beginning of the list
	if ((int)MemoryBlock == curNode->address){
		list_pop(mmu->memory_in_use);
		list_push(mmu->memory_not_in_use, curNode);
		return 0;
	}
	prevNode = curNode;
	curNode = curNode->next;
	while (curNode!=mmu->memory_in_use->end){
		if ((int)MemoryBlock==curNode->address){
			//if found
			prevNode->next = curNode->next;	//remove the node out of cur list
			list_push(mmu->memory_not_in_use, curNode);	//push it on to ther free one
			return 0;
		}
		prevNode = curNode;
		curNode = curNode->next;
	}
	
	return 1;

}
