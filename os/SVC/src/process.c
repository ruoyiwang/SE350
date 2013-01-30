#include "process.h"
#include "processes.h"
#include "memory.h"
#include <LPC17xx.h>
#include "uart_polling.h"

pcb* current_process;
pqueue ready_queue;
pcb_list* pcb_lookup_list;

uint32_t processes[7];
pcb pcbs[7];

pcb_list* root = NULL;
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

void pcb_list_init(pcb_list *root){
    root = NULL;
}

void pcb_insert(pcb *block, pcb_list *root){
    if (root == NULL){
        pcb_list *new_node;
        new_node->process_block = block;
        new_node->next = NULL;
        root = new_node;
    }
    else{
        pcb_insert(block, root->next);
    }
}

pcb* pcb_lookup_by_pid(int pid, pcb_list *root){
    if(root == NULL){
        return NULL;
    }
    else if (pid == root->process_block->pid){
        return root->process_block;
    }
    else{
        return pcb_lookup_by_pid(pid, root->next);
    }
}

int pcb_priority_lookup(int pid, pcb_list *root){
    pcb *node = pcb_lookup_by_pid(pid, root);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }
    return node->priority;
}

void process_init() {
	
	volatile int i, j;
	volatile uint32_t * sp;

	processes[0] = (uint32_t) null_process;
	processes[1] = (uint32_t)test_process_1;
	processes[2] = (uint32_t)test_process_2;
	processes[3] = (uint32_t)test_process_3;
	processes[4] = (uint32_t)test_process_4;
	processes[5] = (uint32_t)test_process_5;
	processes[6] = (uint32_t)test_process_6;
	
	for (i=0; i<7;i++)
	{
		/* initialize the first process	exception stack frame */
		pcbs[i].pid = i;
		pcbs[i].state = NEW;
		pcbs[i].priority = 3;

		sp  = request_memory_block();
			
		/* 8 bytes alignement adjustment to exception stack frame */
		if (!(((uint32_t)sp) & 0x04)) {
				--sp; 
		}
		
		*(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */ 
		*(--sp)  = (uint32_t) processes[i];  /* PC contains the entry point of the process */

		for (j = 0; j < 6; j++) { /* R0-R3, R12 are cleared with 0 */
			*(--sp) = 0x0;
		}
			pcbs[i].sp = sp;
		pqueue_enqueue(&ready_queue,&pcbs[i]);
		//pcb_insert(&pcbs[i], root);
	}

	process_switch();
}

int set_process_priority(int pid, int priority) {
    pcb *node = pcb_lookup_by_pid(pid, root);

    // As per project description section 2.4, if no process exists with the pid passed in return a non-zero int value
    if (node == NULL){
        return -1;
    }
    node->priority = priority;
    return 0;
}

int get_process_priority(int pid) {
    return pcb_priority_lookup(pid, root);
}

int context_switch(pcb* pcb) {
	if (current_process != NULL) {
	    current_process->state = READY;
	    current_process->sp = (uint32_t *) __get_MSP();
	    pqueue_enqueue( &ready_queue, current_process );
	}

  current_process = pcb;
	
	if (current_process->state == NEW) {
    current_process->state = RUN;
    __set_MSP((uint32_t ) current_process->sp);
	current_process->pid = current_process->pid;
		__rte();
	}
	else if (current_process->state == READY) {
    current_process->state = RUN;
    __set_MSP((uint32_t ) current_process->sp);
		current_process->pid = current_process->pid;
	}
	else {
		return 1;
	}
  return 0;
}

int process_switch(){
    pcb* new_process = pqueue_dequeue(&ready_queue);

    // If process queue is empty or the state is not READY execute the null process
    if (new_process == NULL || (new_process->state != READY && new_process->state != NEW)) {
    	new_process = pcb_lookup_by_pid(0,root);
    }

    if (context_switch(new_process)) {
        return 1;
    }

	return 0;
}

// Return 0 if success; 1 if fail
int release_processor() {
	if (current_process == NULL) {
		return 1;
	}

	if (process_switch()) {
		return 1;
	}

    return 0;
}
