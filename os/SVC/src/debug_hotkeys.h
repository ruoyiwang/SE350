#include "interrupt.h"
#ifdef DEBUG_HOTKEYS

typedef unsigned int U32;

extern pqueue ready_queue;

void print_ready_queue_priority(void){
	pcb* pcb;
	// Traverse through each priority queue in the ready queue
	for(int i = 0; i < 4; i ++){
		pcb = ready_queue->pq_front[i];
		while(pcb != NULL){
			if(pcb->state == READY){
#ifdef DEBUG_0
				putchar('0' + pcb->pid);
				putchar(' ');
				putchar('0' + pcb->priority);
#endif
			}
			pcb=pcb->next;
		}
	}
}

void print_memory_blocked_queue_priority(void){
	pcb* pcb;
	// Traverse through each priority queue in the ready queue
	for(int i = 0; i < 4; i ++){
		pcb = ready_queue->pq_front[i];
		while(pcb != NULL){
			if(pcb->state == MEMORY_BLOCK){
#ifdef DEBUG_0
				putchar('0' + pcb->pid);
				putchar(' ');
				putchar('0' + pcb->priority);
#endif
			}
			pcb=pcb->next;
		}
	}
} 

void print_message_blocked_queue_priority(void){
	pcb* pcb;
	// Traverse through each priority queue in the ready queue
	for(int i = 0; i < 4; i ++){
		pcb = ready_queue->pq_front[i];
		while(pcb != NULL){
			if(pcb->state == MESSAGE_BLOCK){
#ifdef DEBUG_0
				putchar('0' + pcb->pid);
				putchar(' ');
				putchar('0' + pcb->priority);
#endif
			}
			pcb=pcb->next;
		}
	}
}
#endif
