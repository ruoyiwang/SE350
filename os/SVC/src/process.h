#ifndef PROCESS_H
#define PROCESS_H

typedef enum {NEW, READY, RUN} process_state;
typedef struct {

} pcb;


pcb* current_process;

void process_init() {

}

int set_process_priority(int pid, int priority) {

}

int get_process_priority(int pid) {
	
}


// Return 0 if success; 1 if fail
int context_switch() {
	current_process->state = READY;
	rq_enqueue(current_processor);
	
}

// Return 0 if success; 1 if fail
int release_processor() {
	if (current_processor = NULL) {
		return 1;
	}

	if (process_switch()) {
		return 1;
	}

	return 0;
}

#endif