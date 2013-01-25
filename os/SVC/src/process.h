#ifndef PROCESS_H
#define PROCESS_H

typedef enum {NEW, READY, RUN} process_state;
typedef struct pcb {

}

void process_init() {

}

int set_process_priority(int pid, int priority) {

}

int get_process_priority(int pid) {
	
}

int context_switch(pcb& proc) {

}

int scheduler() {

}

int process_switch() {

}

int release_processor() {

}

#endif