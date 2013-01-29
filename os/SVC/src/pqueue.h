#ifndef PROCESS_H
#define PROCESS_H

extern struct pcb;
extern enum proc_state_t;

typedef struct {
	proc_state_t state;
	pcb *pq_array[4];
} pqueue;

pcb* pqueue_dequeue(pqueue *queue, int priority)
{
	
}
bool 
#endif