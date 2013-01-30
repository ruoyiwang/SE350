#ifndef NULL
#define NULL (void *)0
#endif

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
typedef enum {NEW, READY, RUN} process_state;

typedef struct pcb_t{
	uint32_t *sp;
	uint32_t pid;
  uint32_t priority;   // priority. Ranges from values 0-4
	process_state state;
	struct pcb_t *next;
	struct pcb_t *prev;       // Pointers to previous and next pcb in the stack
} pcb;

// BST used to store the process ids along with their priorities
typedef struct processBST_t{
    pcb *root;
    struct processBST_t *left;
    struct processBST_t *right;
} processBST;

typedef struct pqueue_t{
	process_state state;
	pcb *pq_end[4];
	pcb *pq_front[4];
} pqueue;

pcb* current_process;
pqueue* ready_queue;

/*************************************************************************
 *          PCB BST DEFINITIONS
 *************************************************************************/
void processBST_init(processBST *root);

processBST* processBST_lookup_by_pid(int pid, processBST* root);

// Priority lookup based on process id
int processBST_priority_lookup(int pid, processBST* root);

void process_init();

int set_process_priority(int pid, int priority);

int get_process_priority(int pid);

int context_switch(pcb* pcb);

int process_switch();

// Return 0 if success; 1 if fail
int release_processor();

#endif
