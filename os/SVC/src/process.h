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
typedef struct pcb_linked_list_t{
    pcb *process_block;
    struct pcb_linked_list_t *next;
} pcb_list;

typedef struct pqueue_t{
	process_state state;
	pcb *pq_end[4];
	pcb *pq_front[4];
} pqueue;

pcb* current_process;
pqueue* ready_queue;
pcb_list* pcb_lookup_list;

// Declare the pcb linked list
pcb_list* root = NULL;
/*************************************************************************
 *          PCB BST DEFINITIONS
 *************************************************************************/
void pcb_list_init(pcb_list *root);

pcb* pcb_lookup_by_pid(int pid, pcb_list *root);

// Insert pcb into linked list
void pcb_insert(pcb *block, pcb_list *root);

// Priority lookup based on process id
int pcb_priority_lookup(int pid, pcb_list *root);

void process_init();

int set_process_priority(int pid, int priority);

int get_process_priority(int pid);

int context_switch(pcb* pcb);

int process_switch();

// Return 0 if success; 1 if fail
int release_processor();

#endif
