#include "process.h"

pcb* pqueue_dequeue(pqueue *queue, int priority)
{
	pcb* ret;
	queue->pq_front[priority] = queue->pq_front[priority]->next;
	ret = queue->pq_front[priority]->prev;
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

void pcb_list_init(pcb_list *root){
    root = NULL;
}

void pcb_insert(pcb *block, pcb_list *root){
    if (root == NULL){
        pcb_list *new_node = new pcb_list;
        new_node->process_block = block;
        new_node->next = NULL;
        root = new_node;
    }
    else{
        pcb_list_insert(block, root->next);
    }
}

pcb_list pcb_lookup_by_pid(int pid, pcb_list *root){
    if(root == NULL){
        return NULL;
    }
    else if (pid == root->process_block->pid){
        return root;
    }
    else{
        return pcb_list_lookup_by_pid(pid, root->next);
    }
}

int pcb_priority_lookup(int pid, pcb_list *root){
    pcb_list *node = pcb_lookup_by_pid(pid, root);

    // As per section 3.5 of project description, if pid is invalid return "-1"
    if(node == NULL){
        return -1;
    }
    return node->process_block->priority;
}

void process_init() {

}

int set_process_priority(int pid, int priority) {
    pcb_list *node = pcb_lookup_by_pid(pid, root);

    // As per project description section 2.4, if no process exists with the pid passed in return a non-zero int value
    if (node == NULL){
        return -1;
    }
    node->process_block->priority = priority;
    return 0;
}

int get_process_priority(int pid) {
    return pcb_priority_lookup(pid, root);
}

int context_switch(pcb* pcb) {
    current_process->state = READY;
    current_process->sp = (uint32_t *) __get_MSP();
    pqueue_enqueue( current_process, current_process );

    current_process = pcb;
    current_process->state = RUN;
    __set_MSP((uint32_t *) current_process->sp);

    return 0;
}

int process_switch(){
    pcb* new_process = pqueue_dequeue(ready_queue,4);

    // If process queue is empty or the state is not READY execute the null process
    if (new_process == NULL || new_process->state != READY) {
    	new_process = pcb_lookup_by_pid(0);
    }

    if (context_switch(new_process)) {
        return 1;
    }

	return 0;
}

// Return 0 if success; 1 if fail
int release_processor() {
	if (current_process == NULL) {
		return 1;
	}

	if (process_switch()) {
		return 1;
	}

    return 0;
}
