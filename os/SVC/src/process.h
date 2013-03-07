#ifndef NULL
#define NULL (void *)0
#endif

#ifndef PROCESS_H
#define PROCESS_H

typedef unsigned int U32;
#define __SVC_0  __svc_indirect(0)

#include <stdint.h>
typedef enum {NEW, READY, MEMORY_BLOCK, MESSAGE_BLOCK, RUN, INTERRUPT} process_state;
typedef enum {DISPLAY_REQUEST, COMMAND_REGISTRATION, KEYBOARD_REQUEST} message_type;

typedef struct envelope_t{
	void* message;
	message_type type;
	int src_id;
	int dest_id;
	int message_length;
	envelope_t* next;
} envelope;

typedef struct mailbox_t{
	envelope* front;
	envelope* end;
} mailbox;

typedef struct pcb_t{
	volatile uint32_t pc;
	uint32_t *sp;
	volatile uint32_t pid;
  volatile uint32_t priority;   // priority. Ranges from values 0-4
	volatile process_state state;
	struct pcb_t *next;
	struct pcb_t *prev;       // Pointers to previous and next pcb in the stack
	struct pcb_t *lu_next;
	mailbox mb;
} pcb;

typedef struct pqueue_t{
	volatile process_state state;
	pcb *pq_end[4];
	pcb *pq_front[4];
} pqueue;

/*************************************************************************
 *          PCB LINKED LIST DEFINITIONS
 *************************************************************************/
pcb* pcb_lookup_by_pid(int pid, pcb *node);
//#define pcb_lookup_by_pid(pid, node) _pcb_lookup_by_pid((U32)k_pcb_lookup_by_pid, pid, node)
//pcb* _pcb_lookup_by_pid(U32 p_func, int pid, pcb *node)  __SVC_0;

// Insert pcb into linked list
void pcb_insert(pcb *block, pcb *node);

void process_init(void);

pcb* pqueue_dequeue(pqueue *queue);
//#define pqueue_dequeue(queue) _pqueue_dequeue((U32)k_pqueue_dequeue, queue)
//pcb* _pqueue_dequeue(U32 p_func, pqueue *queue)  __SVC_0;

void pqueue_enqueue(pqueue *queue, pcb *new_pcb);

void atomic(bool onOff);
//#define pqueue_enqueue(new_pcb, queue) _pqueue_enqueue((U32)k_pqueue_enqueue, queue, new_pcb)
//int __SVC_0 _pqueue_enqueue(U32 p_func, pqueue *queue, pcb *new_pcb);

void k_send_message(int dest_id, envelope* env);
#define send_message(dest_id, env) _send_message((U32)k_send_message, dest_id, env)
void __SVC_0 _send_message(U32 p_func, int dest_id, envelope* env);

envelope* k_receive_message();
#define receive_message() _receive_message((U32)k_receive_message)
envelope* __SVC_0 _receive_message(U32 p_func);

int k_get_process_priority(int pid);
#define get_process_priority(pid) _get_process_priority((U32)k_get_process_priority, pid)
int __SVC_0 _get_process_priority(U32 p_func, int pid);

extern int k_context_switch(pcb* pcb);
#define context_switch(pcb) _context_switch((U32)k_context_switch, pcb)
int __SVC_0 _context_switch(U32 p_func, pcb* pcb);

// Return 0 if success; 1 if fail
extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
int __SVC_0 _release_processor(U32 p_func);

void block_current_process(void);

extern void __rte(void);
extern void null_process(void);
extern void test_process_1(void);
extern void test_process_2(void);
extern void test_process_3(void);
extern void test_process_4(void);
extern void test_process_5(void);
extern void test_process_6(void);

#endif
