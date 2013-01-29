#ifndef NULL
#define NULL (void *)0
#endif

#ifndef PROCESS_H
#define PROCESS_H

#include "pqueue.h"

typedef enum {NEW, READY, RUN} process_state;
typedef struct {

} pcb;


pcb* current_process;
pqueue* ready_queue;

void process_init() {

}

int set_process_priority(int pid, int priority) {
    pcb process = pcbLookup(pid);

    process.priority = priority;
}

int get_process_priority(int pid) {
    pcb process = pcbLookup(pid);

    return process.priority;
}

int process_switch(){
	current_process->state = READY;
	current_process->sp = (uint32_t *) __get_MSP();
	pqueue_enqueue(current_process, current_process->priority);

    // Go through process queues that are 'ready' and take the highest priority process
    current_process = pqueue_dequeue(ready_queue);

    // If process queue is empty or the state is not READY execute the null process
    if (current_process == NULL || current_process->state != READY) {
    	current_process = pcb_lookup(0);
    }

    current_process->state = RUN;
    __set_MSP((uint32_t *) current_process->sp);

	return 0;
}

// Return 0 if success; 1 if fail
int release_processor() {
	if (current_process = NULL) {
		return 1;
	}

	if (process_switch()) {
		return 1;
	}
}

#endif
