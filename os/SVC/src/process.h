#ifndef PROCESS_H
#define PROCESS_H

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



pcb* current_processor;

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

int context_switch(pcb& proc) {

}

int scheduler() {

}

void process_switch(){
    // Go through process queues that are 'ready' and take the highest priority process

    // If process queue is empty execute the null process

}

int release_processor() {
	current_processor->state = READY;
	rq_enqueue(current_processor);
	process_switch();
}

#endif
