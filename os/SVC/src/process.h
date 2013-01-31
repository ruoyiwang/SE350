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
	volatile uint32_t pc;
	uint32_t *sp;
	volatile uint32_t pid;
  uint32_t priority;   // priority. Ranges from values 0-4
	volatile process_state state;
	struct pcb_t *next;
	struct pcb_t *prev;       // Pointers to previous and next pcb in the stack
	struct pcb_t *lu_next;
} pcb;

typedef struct pqueue_t{
	process_state state;
	pcb *pq_end[4];
	pcb *pq_front[4];
} pqueue;

extern pcb* current_process;
extern pqueue ready_queue;
extern pcb* pcb_lookup_list;

/*************************************************************************
 *          PCB LINKED LIST DEFINITIONS
 *************************************************************************/
void k_pcb_list_init(pcb *node);
#define pcb_list_init(node) _pcb_list_init((U32)pcb_list_init, node)
pcb* _pcb_list_init(U32 p_func, pcb *node)  __SVC_0;

pcb* k_pcb_lookup_by_pid(int pid, pcb *node);
#define pcb_lookup_by_pid(pid, node) _pcb_lookup_by_pid((U32)pcb_lookup_by_pid, pid, node)
pcb* _pcb_lookup_by_pid(U32 p_func, int pid, pcb *node)  __SVC_0;

// Insert pcb into linked list
void pcb_insert(pcb *block, pcb *node);

// Priority lookup based on process id
int k_pcb_priority_lookup(int pid);
#define pcb_priority_lookup(pid) _pcb_priority_lookup((U32)pcb_priority_lookup, pid)
int _pcb_priority_lookup(U32 p_func, int pid)  __SVC_0;

void process_init();

pcb* k_pqueue_dequeue(pqueue *queue);
#define pqueue_dequeue() _pqueue_dequeue((U32)k_pqueue_dequeue)
pcb* _pqueue_dequeue(U32 p_func)  __SVC_0;

void k_pqueue_enqueue(pqueue *queue, pcb *new_pcb);
#define pqueue_enqueue(new_pcb) _pqueue_enqueue((U32)k_pqueue_enqueue, new_pcb)
int __SVC_0 _pqueue_enqueue(U32 p_func, pcb *new_pcb);

int k_set_process_priority(int pid, int priority);
#define set_process_priority(pid, priority) _set_process_priority((U32)k_set_process_priority, pid, priority)
int __SVC_0 _set_process_priority(U32 p_func, int pid, int priority);

int k_get_process_priority(int pid);
#define get_process_priority(pid) _get_process_priority((U32)k_get_process_priority, pid)
int __SVC_0 _get_process_priority(U32 p_func, int pid);

extern int k_context_switch(pcb* pcb);
#define context_switch(pcb) _context_switch((U32)k_context_switch, pcb)
int __SVC_0 _context_switch(U32 p_func, pcb* pcb);

extern int k_process_switch();
#define process_switch() _process_switch((U32)k_process_switch)
int __SVC_0 _process_switch(U32 p_func);

// Return 0 if success; 1 if fail
extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
int __SVC_0 _release_processor(U32 p_func);

extern void __rte(void);
extern void null_process(void);
extern void test_process_1(void);
extern void test_process_2(void);
extern void test_process_3(void);
extern void test_process_4(void);
extern void test_process_5(void);
extern void test_process_6(void);

#endif
