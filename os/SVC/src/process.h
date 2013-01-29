#ifndef PROCESS_H
#define PROCESS_H

typedef enum {NEW, READY, RUN} process_state;
typedef struct {

} pcb;


pcb* current_processor;

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

int context_switch(pcb& proc) {

}

int scheduler() {

}

void processSwitch(){
    // Go through process queues that are 'ready' and take the highest priority process

    // If process queue is empty execute the null process
    null_process();
}

int release_processor() {
	current_processor->state = READY;
	rq_enqueue(current_processor);
	process_switch();
}

#endif
