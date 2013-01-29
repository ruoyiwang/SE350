#ifndef PROCESS_H
#define PROCESS_H
#define NULL 0

#include "pqueue.h"

typedef enum {NEW, READY, RUN} process_state;
typedef struct {
    int pid;        // process id
    int priority;   // priority. Ranges from values 0-4
    uint32_t *sp;   // Stack pointer

} pcb;

// BST used to store the process ids along with their priorities
typedef struct{
    pcb *root;
    processBST *left;
    processBST *right;
} processBST;

/*************************************************************************
 *          PCB BST DEFINITIONS
 *************************************************************************/
void processBST_init(processBST& root){
    root = NULL;
}

processBST processBST_lookup_by_pid(int pid, processBST& root){
    if(root == NULL){
        return NULL;
    }
    else if (pid == root->pcb->pid){
        return root;
    }
    else if(pid < root->pcb->pid){
        return processBST_lookup_by_pid(pid, root->left);
    }
    else{
        return processBST_lookup_by_pid(pid, root->right);
    }
}
// Priority lookup based on process id
int processBST_priority_lookup(int pid, processBST& root){
    processBST *node = processBST_lookup_by_pid(pid, root);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }
    return node->pcb->priority;
}



pcb* current_process;
pqueue* ready_queue;

void process_init() {

}

int set_process_priority(int pid, int priority) {
    processBST *node = processBST_lookup_by_pid(pid, root);

    // As per project description section 2.4, if no process exists with the pid passed in return a non-zero int value
    if (node == NULL){
        return -1;
    }
    node->pcb->priority = priority;
    return 0;
}

int get_process_priority(int pid) {
    return processBST_priority_lookup(pid, root);
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
