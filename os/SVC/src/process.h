#ifndef NULL
#define NULL (void *)0
#endif

#ifndef PROCESS_H
#define PROCESS_H

typedef unsigned int U32;
#define __SVC_0  __svc_indirect(0)

#include <stdint.h>
typedef enum {NEW, READY, RUN} process_state;

typedef struct pcb_t{
	uint32_t *sp;
	volatile uint32_t pid;
  uint32_t priority;   // priority. Ranges from values 0-4
	volatile process_state state;
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

extern pcb* current_process;
extern pqueue ready_queue;
extern pcb_list* pcb_lookup_list;

// Declare the pcb linked list
extern pcb_list* root;
/*************************************************************************
 *          PCB BST DEFINITIONS
 *************************************************************************/
void pcb_list_init(pcb_list *root);
#define k_pcb_list_init(root) _pcb_list_init((U32)pcb_list_init, root)
pcb* _pcb_list_init(U32 p_func, pcb_list *root)  __SVC_0;

pcb* pcb_lookup_by_pid(int pid, pcb_list *root);
#define k_pcb_lookup_by_pid(pid, root) _pcb_lookup_by_pid((U32)pcb_lookup_by_pid, pid, root)
pcb* _pcb_lookup_by_pid(U32 p_func, int pid, pcb_list *root)  __SVC_0;

// Insert pcb into linked list
void pcb_insert(pcb *block, pcb_list *root);

// Priority lookup based on process id
int pcb_priority_lookup(int pid, pcb_list *root);
#define k_pcb_priority_lookup(pid, root) _pcb_priority_lookup((U32)pcb_priority_lookup, pid, root)
int _pcb_priority_lookup(U32 p_func, int pid, pcb_list *root)  __SVC_0;

void process_init();

pcb* pqueue_dequeue(pqueue *queue);
#define k_pqueue_dequeue() _pqueue_dequeue((U32)pqueue_dequeue)
pcb* _pqueue_dequeue(U32 p_func)  __SVC_0;

void pqueue_enqueue(pqueue *queue, pcb *new_pcb);
#define k_pqueue_enqueue(new_pcb) _pqueue_enqueue((U32)pqueue_enqueue, new_pcb)
int __SVC_0 _pqueue_enqueue(U32 p_func, pcb *new_pcb);

int set_process_priority(int pid, int priority);
#define k_set_process_priority(pid, priority) _set_process_priority((U32)set_process_priority, pid, priority)
int __SVC_0 _set_process_priority(U32 p_func, int pid, int priority);

int get_process_priority(int pid);
#define k_get_process_priority(pid) _get_process_priority((U32)get_process_priority, pid)
int __SVC_0 _get_process_priority(U32 p_func, int pid);

extern int context_switch(pcb* pcb);
#define k_context_switch(pcb) _context_switch((U32)context_switch, pcb)
int __SVC_0 _context_switch(U32 p_func, pcb* pcb);

extern int process_switch();
#define k_process_switch() _process_switch((U32)process_switch)
int __SVC_0 _process_switch(U32 p_func);

// Return 0 if success; 1 if fail
extern int release_processor(void);
#define kk_release_processor() _release_processor((U32)release_processor)
int __SVC_0 _release_processor(U32 p_func);

extern void __rte(void);

#endif
