#ifndef PROCESS_H
#define PROCESS_H

typedef enum {NEW, READY, RUN} process_state;
typedef struct {

} pcb;


pcb* current_process;

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
    // Go through process queues that are 'ready' and take the highest priority process

    // If process queue is empty execute the null process
    null_process();
	return 0;
}

// Return 0 if success; 1 if fail
int release_processor() {
	if (current_processor = NULL) {
		return 1;
	}

	if (process_switch()) {
		return 1;
	}
}

#endif
