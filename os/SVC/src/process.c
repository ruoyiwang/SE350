#include "process.h"
#include "interrupt.h"
#include "timer.h"
#include "memory.h"
#include "keyboard.h"
#include <LPC17xx.h>
#include "uart_polling.h"
#include "CRT.h"
#define NUM_PROCS 10

pcb* current_process;
pcb *pcb_lookup_list;
mailbox* delay_message_list;
pqueue ready_queue;
MMU mmu;
pcb* pcbs[NUM_PROCS];
i_process interrupt_process;
i_process timer;

void atomic (int onOff)
{
	if (onOff)
	{
  		__disable_irq();
	}
	else
	{
		__enable_irq();
	}
}
void k_send_message(int dest_id, envelope* env)
{
	pcb* dest_pcb;
	int i;
	atomic(1);
	for ( i= 0; i<NUM_PROCS; i++)
	{
		if (pcbs[i]->pid == dest_id)
		{
			dest_pcb = pcbs[i];
			break;
		}
	}
	env->expire_time = 0;
	env->dest_id = dest_id;
	env->src_id = current_process->pid;
	if (dest_pcb->mb.front == NULL)
	{
		dest_pcb->mb.front = env;
		dest_pcb->mb.end = env;
	}
	else
	{
		dest_pcb->mb.end->next = env;
		dest_pcb->mb.end = env;
	}
	if (dest_pcb->state == MESSAGE_BLOCK)
	{
		dest_pcb->state = READY;
	}
	atomic(0);
}

envelope* k_receive_message()
{
	envelope* ret;
	atomic(1);
	while (current_process->mb.front == NULL)
	{
		current_process->state = MESSAGE_BLOCK;
		atomic(0);
		k_release_processor();
		atomic(1);
	}
	if (current_process->mb.front == current_process->mb.end)
	{
		ret = current_process->mb.front;
		current_process->mb.front = NULL;
		current_process->mb.end = NULL;
	}
	else
	{
		ret = current_process->mb.front;
		current_process->mb.front = current_process->mb.front->next;
	}
	ret->next = NULL;
	atomic(0);
	return ret;
}

void k_delay_send( int dest_id, envelope *env, int delay)
{
	atomic(1);
	env->expire_time = g_timer_count + delay;
	env->dest_id = dest_id;
	env->src_id = current_process->pid;
	if (delay_message_list->end->next == NULL)
	{
		delay_message_list->front = env;
		delay_message_list->end = env;
	}
	else
	{
		delay_message_list->end->next = env;
		delay_message_list->end = env;
	}
	atomic(0);
}

pcb* pqueue_dequeue(pqueue *queue)
{
	pcb* ret;
	int i;
	pcb* before;
	pcb* after;
	for (i=0; i<4; i++)
	{
		if (queue->pq_front[i] == NULL)
				continue;
		ret = queue->pq_front[i];
		if (!mmu.memory_available)
		{
			while ( (ret->state == MEMORY_BLOCK||ret->state == MESSAGE_BLOCK) && ret->next != NULL)
			{
				ret= ret->next;
			}
			if(ret == queue->pq_end[i] && (ret->state == MEMORY_BLOCK||ret->state == MESSAGE_BLOCK))
					continue;
		}
		else
		{
			while (ret->state == MESSAGE_BLOCK && ret->next != NULL)
			{
				ret= ret->next;
			}
			if(ret == queue->pq_end[i] && ret->state == MESSAGE_BLOCK)
					continue;			
		}
		if (ret == queue->pq_front[i] && ret == queue->pq_end[i])
		{
			queue->pq_front[i] = NULL;
			queue->pq_end[i] = NULL;
		}
		else if (ret == queue->pq_front[i])
		{
			queue->pq_front[i] = queue->pq_front[i]->next;
			queue->pq_front[i]->prev = NULL;
		}
		else if (ret == queue->pq_end[i])
		{
			queue->pq_end[i] = queue->pq_end[i]->prev;
			queue->pq_end[i]->next = NULL;
		}
		else
		{
			before = ret->prev;
			after = ret->next;
			before->next = after;
			after->prev = before;
		}

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
	new_pcb->next = NULL;
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
    /*if(node == NULL){
        return NULL;
    }
    else if (pid == node->pid){
        return node;
    }
    else{
        return pcb_lookup_by_pid(pid, node->lu_next);
    }*/
	if (pid > NUM_PROCS || pid < 0) {
			return NULL;
		}
	return pcbs[pid];
}

void process_init() {

	volatile int i, j;
	uint32_t * sp;
	
	for (i=0; i < NUM_PROCS; i++) {
		pcbs[i] = (pcb*)k_request_memory_block();
	}
	pcbs[0]->pc = (uint32_t)null_process;
	pcbs[1]->pc = (uint32_t)test_process_1;
	pcbs[2]->pc = (uint32_t)test_process_2;
	pcbs[3]->pc = (uint32_t)test_process_3;
	pcbs[4]->pc = (uint32_t)test_process_4;
	pcbs[5]->pc = (uint32_t)test_process_5;
	pcbs[6]->pc = (uint32_t)test_process_6;
	pcbs[7]->pc = (uint32_t)kcd;
	pcbs[8]->pc = (uint32_t)crt_displpay_process;
	pcbs[9]->pc = (uint32_t)wall_clock;

	pcbs[0]->priority = 3;
	pcb_lookup_list = pcbs[0];

	// setup the interrupt process;
	interrupt_process.pcb.pid = 10;
	interrupt_process.pcb.pc = (uint32_t)i_process_routine;
	interrupt_process.state = RUNNING;
	interrupt_process.pcb.priority=0;
	interrupt_process.pcb.state=INTERRUPT;
	interrupt_process.pcb.mb.front = NULL;
	interrupt_process.pcb.mb.end = NULL;
	interrupt_process.pcb.next = NULL;
	interrupt_process.pcb.lu_next = NULL;
	interrupt_process.pcb.prev = NULL;
	sp = k_request_memory_block();
	sp = sp + USR_SZ_STACK - 4;
	// 8 bytes alignement adjustment to exception stack frame 
	if (!(((uint32_t)sp) & 0x04)) {
			--sp;
	}

	*(--sp)  = INITIAL_xPSR;      // user process initial xPSR 
	*(--sp)  = (uint32_t)interrupt_process.pcb.pc;  // PC contains the entry point of the process 
	for (j = 0; j < 6; j++) { // R0-R3, R12 are cleared with 0 
		*(--sp) = 0x0;
	}
	interrupt_process.pcb.sp = sp;

	timer.pcb.pid = 11;
	timer.pcb.pc = (uint32_t)timer_iprocess;
	timer.state = RUNNING;
	timer.pcb.priority=0;
	timer.pcb.state=INTERRUPT;
	timer.pcb.mb.front = NULL;
	timer.pcb.mb.end = NULL;
	timer.pcb.next = NULL;
	timer.pcb.lu_next = NULL;
	timer.pcb.prev = NULL;
	sp = k_request_memory_block();
	sp = sp + USR_SZ_STACK - 4;
	// 8 bytes alignement adjustment to exception stack frame 
	if (!(((uint32_t)sp) & 0x04)) {
			--sp;
	}

	*(--sp)  = INITIAL_xPSR;      // user process initial xPSR 
	*(--sp)  = (uint32_t)timer.pcb.pc;  // PC contains the entry point of the process 
	for (j = 0; j < 6; j++) { // R0-R3, R12 are cleared with 0 
		*(--sp) = 0x0;
	}
	timer.pcb.sp = sp;

	for (i=0; i<NUM_PROCS;i++)
	{
		/* initialize the first process	exception stack frame */
		pcbs[i]->pid = i;
		pcbs[i]->state = NEW;
		pcbs[i]->next = NULL;
		pcbs[i]->lu_next = NULL;
		pcbs[i]->mb.front = NULL;
		pcbs[i]->mb.end = NULL;
		pcbs[i]->prev = NULL;
		
		if (i == 7 || i ==8 || i == 9)
			pcbs[i]->priority = 1;
		else if (i!=0)
			pcbs[i]->priority = 2;

		sp = k_request_memory_block();
		sp = sp + USR_SZ_STACK - 4;
		/* 8 bytes alignement adjustment to exception stack frame */
		if (!(((uint32_t)sp) & 0x04)) {
				--sp;
		}

		*(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */
		*(--sp)  = (uint32_t)pcbs[i]->pc;  /* PC contains the entry point of the process */

		for (j = 0; j < 6; j++) { /* R0-R3, R12 are cleared with 0 */
			*(--sp) = 0x0;
		}

		pcbs[i]->sp = sp;
		pqueue_enqueue(&ready_queue,pcbs[i]);
		if (i!=0)
			pcb_insert(pcbs[i], pcb_lookup_list);
	}
}

int k_set_process_priority(int pid, int priority) {
    pcb *node = pcb_lookup_by_pid(pid, pcb_lookup_list);

    // As per project description section 2.4, if no process exists with the pid passed in return a non-zero int value
    if (node == NULL){
        return -1;
    }
		k_pqueue_set_priority(&ready_queue, node, priority);

		return 0;
}

int k_get_process_priority(int pid){
    /*pcb *node = pcb_lookup_by_pid(pid, pcb_lookup_list);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }
		*/
	
		if (pid > NUM_PROCS || pid < 0) {
			return -1;
		}
		return pcbs[pid]->priority;
}

int k_context_switch(pcb* pcb) {
	if (current_process != NULL) {
			if (current_process->state != MESSAGE_BLOCK) {
				current_process->state = READY;
			}
			current_process->sp = (uint32_t *) __get_MSP();
			if (pcb->state != INTERRUPT) {
				pqueue_enqueue( &ready_queue, current_process );
			}
	}

	current_process = pcb;

	if (current_process->state == NEW) {
	    current_process->state = RUN;
	    __set_MSP((uint32_t ) current_process->sp);
		__rte();
	}
	else if (current_process->state == READY || current_process->state == INTERRUPTED) {
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
		pcb* new_process = NULL;
    new_process = pqueue_dequeue(&ready_queue);

    // If process queue is empty or the state is not READY execute the null process
    if (new_process == NULL || (new_process->state != READY && new_process->state != NEW && new_process->state != MEMORY_BLOCK && new_process->state != MESSAGE_BLOCK)) {
    	new_process = pcb_lookup_by_pid(0, pcb_lookup_list);
    }
		
    if (k_context_switch(new_process)) {
        return 1;
    }
		
    return 0;
}

void block_current_process()
{
	current_process->state = MEMORY_BLOCK;
}

void print_ready_queue_priority(void){
	pcb* pcb;
	int i;
	// Traverse through each priority queue in the ready queue
	for(i = 0; i < NUM_PROCS; i ++){
		pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
		if(pcb->state == READY){
			uart0_put_char('\n');
			uart0_put_char('0' + pcb->pid);
			uart0_put_char(' ');
			uart0_put_char('0' + pcb->priority);
		}
	}
}

void print_memory_blocked_queue_priority(void){
	pcb* pcb;
	int i;
	// Traverse through each priority queue in the ready queue
	for(i = 0; i < NUM_PROCS; i ++){
		pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
		if(pcb->state == MEMORY_BLOCK){
			uart0_put_char('\n');
			uart0_put_char('0' + pcb->pid);
			uart0_put_char(' ');
			uart0_put_char('0' + pcb->priority);
		}
	}
} 

void print_message_blocked_queue_priority(void){
	pcb* pcb;
	int i;
	// Traverse through each priority queue in the ready queue
	for(i = 0; i < NUM_PROCS; i ++){
		pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
		if(pcb->state == MESSAGE_BLOCK){
			uart0_put_char('\n');
			uart0_put_char('0' + pcb->pid);
			uart0_put_char(' ');
			uart0_put_char('0' + pcb->priority);
		}
	}
}

