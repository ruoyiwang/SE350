#ifndef NULL
#define NULL (void *)0
#endif

#ifndef PROCESS_H
#define PROCESS_H

typedef unsigned int U32;
#define __SVC_0  __svc_indirect(0)

#include <stdint.h>
typedef enum {NEW, READY, MEMORY_BLOCK, MESSAGE_BLOCK, RUN, INTERRUPT, INTERRUPTED} process_state;
typedef enum {DISPLAY_REQUEST, COMMAND_REGISTRATION, KEYBOARD_INPUT, TIMER_UPDATE} message_type;

typedef struct envelope_t{
	void* message;
	message_type type;
	uint32_t expire_time;
	int src_id;
	int dest_id;
	int message_length;
	struct envelope_t* next;
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

extern pcb* current_process;

extern pcb *pcb_lookup_list;

extern mailbox* delay_message_list;
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

void atomic(int onOff);
//#define pqueue_enqueue(new_pcb, queue) _pqueue_enqueue((U32)k_pqueue_enqueue, queue, new_pcb)
//int __SVC_0 _pqueue_enqueue(U32 p_func, pqueue *queue, pcb *new_pcb);

extern void k_send_message(int dest_id, envelope* env);
#define send_message(dest_id, env) _send_message((U32)k_send_message, dest_id, env)
extern void __SVC_0 _send_message(U32 p_func, int dest_id, envelope* env);

extern void k_delay_send(int dest_id, envelope* env, int delay);
#define delay_send(dest_id, env, delay) _delay_send((U32)k_delay_send, dest_id, env, delay)
extern void __SVC_0 _delay_send(U32 p_func, int dest_id, envelope* env, int delay);

extern envelope* k_receive_message(void);
#define receive_message() _receive_message((U32)k_receive_message)
extern envelope* _receive_message(U32 p_func) __SVC_0;

extern int k_set_process_priority(int pid, int priority);
#define set_process_priority(pid,priority) _set_process_priority((U32)k_set_process_priority, pid, priority)
extern int __SVC_0 _set_process_priority(U32 p_func, int pid, int priority);

extern int k_get_process_priority(int pid);
#define get_process_priority(pid) _get_process_priority((U32)k_get_process_priority, pid)
extern int __SVC_0 _get_process_priority(U32 p_func, int pid);

extern int k_context_switch(pcb* pcb);
#define context_switch(pcb) _context_switch((U32)k_context_switch, pcb)
extern int __SVC_0 _context_switch(U32 p_func, pcb* pcb);

// Return 0 if success; 1 if fail
extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

void block_current_process(void);

extern void print_ready_queue_priority(void);
extern void	print_memory_blocked_queue_priority(void);
extern void	print_message_blocked_queue_priority(void);

extern void __rte(void);
extern void null_process(void);
extern void test_process_1(void);
extern void test_process_2(void);
extern void test_process_3(void);
extern void test_process_4(void);
extern void test_process_5(void);
extern void test_process_6(void);
extern void i_process_routine(void);
extern void timer_iprocess(void);
extern void kcd(void);
extern void wall_clock(void);

#endif
