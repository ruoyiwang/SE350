#ifndef NULL
#define NULL ((void *)0) 
#endif

#ifndef PROCESS_H
#define PROCESS_H

extern struct pcb;
extern enum proc_state_t;

typedef struct {
	proc_state_t state;
	pcb *pq_end[4];
	pcb *pq_front[4];
} pqueue;

#endif