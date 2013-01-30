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

void processBST_init(processBST &root) {
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

int processBST_priority_lookup(int pid, processBST& root){
    processBST *node = processBST_lookup_by_pid(pid, root);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }
    return node->pcb->priority;
}

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

int context_switch(pcb* pcb) {
    current_process->state = READY;
    current_process->sp = (uint32_t *) __get_MSP();
    pqueue_enqueue(current_process, current_process->priority);

    current_process = pcb;
    current_process->state = RUN;
    __set_MSP((uint32_t *) current_process->sp);

    return 0;
}

int process_switch(){
    pcb* new_process = pqueue_dequeue(ready_queue,4);

    // If process queue is empty or the state is not READY execute the null process
    if (new_process == NULL || new_process->state != READY) {
    	new_process = pcb_lookup(0);
    }

    if (context_switch(new_process)) {
        return 1;
    }

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

    return 0;
}
