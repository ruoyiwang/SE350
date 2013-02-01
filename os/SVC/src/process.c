#include "process.h"
#include "memory.h"
#include <LPC17xx.h>
#include "uart_polling.h"

pcb* current_process;
pqueue ready_queue;
pcb *pcb_lookup_list;

pcb pcbs[7];

MMU mmu;

pcb* pqueue_dequeue(pqueue *queue)
{
	pcb* ret;
	int i;

	for (i=0; i<4; i++)
	{
		queue->pq_front[i]->state = queue->pq_front[i]->state;
		queue->pq_end[i]->pid = queue->pq_end[i]->pid;
		if (queue->pq_front[i] == NULL)
				continue;
		ret = queue->pq_front[i];
		queue->pq_front[i] = queue->pq_front[i]->next;
		queue->pq_front[i]->prev = NULL;
		return ret;
	}
	return NULL;
}

void pqueue_enqueue(pqueue *queue, pcb *new_pcb)
{
	volatile int priority = new_pcb->priority;
	if (queue->pq_front[priority] == NULL)
	{
		queue->pq_front[priority] = new_pcb;
		queue->pq_end[priority] = new_pcb;
		return;
	}
	queue->pq_end[priority]->next = new_pcb;
	new_pcb->prev = queue->pq_end[priority];
	queue->pq_end[priority] = new_pcb;
}

void pcb_insert(pcb *block, pcb *node){
    pcb *iterator;
    block->lu_next = NULL;
    if (node == NULL){
        node = block;
        return;
    }
    iterator = node;
    while(iterator->lu_next != NULL){
        iterator = iterator->lu_next;
    }
    iterator->lu_next = block;
}

void k_pqueue_set_priority(pqueue *queue, pcb *_pcb, int priority)
{
	pcb* before = _pcb->prev;
	pcb* after = _pcb->next;
	if (_pcb->priority == priority)
		return;
	if (after == NULL && before == NULL)
	{
		_pcb->priority = priority;
	}
	if (after == NULL)
	{
		queue->pq_end[_pcb->priority] = before;
		before->next = NULL;
		_pcb->priority = priority;
		pqueue_enqueue(queue,_pcb); 
	}
	else if (before == NULL)
	{
		queue->pq_front[_pcb->priority] = after;
		after->prev = NULL;
		_pcb->priority = priority;
		pqueue_enqueue(queue,_pcb); 
	}
	else 
	{
		before->next = after;
		after->prev = before;
		_pcb->priority = priority;
		pqueue_enqueue(queue,_pcb); 
	}
	
}

pcb* pcb_lookup_by_pid(int pid, pcb *node){
    if(node == NULL){
        return NULL;
    }
    else if (pid == node->pid){
        return node;
    }
    else{
        return pcb_lookup_by_pid(pid, node->lu_next);
    }
}

void process_init() {

	volatile int i, j;
	uint32_t * sp;
	pcbs[0].pc = (uint32_t)null_process;
	pcbs[1].pc = (uint32_t)test_process_1;
	pcbs[2].pc = (uint32_t)test_process_2;
	pcbs[3].pc = (uint32_t)test_process_3;
	pcbs[4].pc = (uint32_t)test_process_4;
	pcbs[5].pc = (uint32_t)test_process_5;
	pcbs[6].pc = (uint32_t)test_process_6;

	pcbs[0].priority = 3;
	pcb_lookup_list = &pcbs[0];
	for (i=0; i<7;i++)
	{
		/* initialize the first process	exception stack frame */
		pcbs[i].pid = i;
		pcbs[i].state = NEW;
		if (i!=0)
			pcbs[i].priority = 2;

		sp  = k_request_memory_block();
		/* 8 bytes alignement adjustment to exception stack frame */
		if (!(((uint32_t)sp) & 0x04)) {
				--sp;
		}

		*(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */
		*(--sp)  = (uint32_t)pcbs[i].pc;  /* PC contains the entry point of the process */

		for (j = 0; j < 6; j++) { /* R0-R3, R12 are cleared with 0 */
			*(--sp) = 0x0;
		}

		pcbs[i].sp = sp;
		pqueue_enqueue(&ready_queue,&pcbs[i]);
		if (i!=0)
			pcb_insert(&pcbs[i], pcb_lookup_list);
	}
}

int k_set_process_priority(int pid, int priority) {
    pcb *node = pcb_lookup_by_pid(pid, pcb_lookup_list);

    // As per project description section 2.4, if no process exists with the pid passed in return a non-zero int value
    if (node == NULL){
        return -1;
    }
    node->priority = priority;
	k_pqueue_set_priority(&ready_queue, node, priority);
   //pcbs[pid].priority = priority;
		return 0;
}

int k_get_process_priority(int pid){
    /*pcb *node = pcb_lookup_by_pid(pid, pcb_lookup_list);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }*/

	return pcbs[pid].priority;
}

int k_context_switch(pcb* pcb) {
	if (current_process != NULL) {
	    current_process->state = READY;
	    current_process->sp = (uint32_t *) __get_MSP();
	    pqueue_enqueue( &ready_queue, current_process );
	}

  current_process = pcb;

	if (current_process->state == NEW) {
    current_process->state = RUN;
    __set_MSP((uint32_t ) current_process->sp);
		__rte();
	}
	else if (current_process->state == READY) {
    current_process->state = RUN;
    __set_MSP((uint32_t ) current_process->sp);
	}
	else {
		return 1;
	}
  return 0;
}

// Return 0 if success; 1 if fail
int k_release_processor() {
    pcb* new_process = pqueue_dequeue(&ready_queue);

    // If process queue is empty or the state is not READY execute the null process
    if (new_process == NULL || (new_process->state != READY && new_process->state != NEW && new_process->state != BLOCKED)) {
    	new_process = pcb_lookup_by_pid(0, pcb_lookup_list);
    }
		else if (new
		
    if (k_context_switch(new_process)) {
        return 1;
    }

    return 0;
}

void k_block_current_process()
{
	current_process->state = BLOCK;
}
