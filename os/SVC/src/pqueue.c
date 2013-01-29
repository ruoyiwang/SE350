#include "pqueue.h"
#include "process.h"

pcb* pqueue_dequeue(pqueue *queue, int priority)
{
	queue->pq_front[priority] = queue->pq_front[priority]->next;
	pcb* ret = queue->pq_front[priority]->prev;
	queue->pq_front[priority]->prev = NULL;
	return ret;
}

void pqueue_enqueue(pqueue *queue, pcb *new_pcb)
{
	volatile int priority = new_pcb->priority;
	queue->pq_end[priority]->next = new_pcb;
	new_pcb->prev = queue->pq_end[priority];
	queue->pq_end[priority] = new_pcb;
} 