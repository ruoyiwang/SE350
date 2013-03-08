#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "process.h"
#include "debug_hotkeys.h"
#include <stdint.h>	/* typedefs */

#ifndef DEBUG_0
#include <stdio.h>
#endif

/* The following macros are from NXP uart.h */
#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define BUFSIZE		0x40
/* end of NXP uart.h file reference */

#define BIT(X)    ( 1<<X )

/* 
   8 bits, no Parity, 1 Stop bit
   
   0x83 = 1000 0011 = 1 0 00 0 0 11
   LCR[7]  =1  enable Divisor Latch Access Bit DLAB
   LCR[6]  =0  disable break transmission
   LCR[5:4]=00 odd parity
   LCR[3]  =0  no parity
   LCR[2]  =0  1 stop bit
   LCR[1:0]=11 8-bit char len
   See table 279, pg306 LPC17xx_UM
*/
#define UART_8N1  0x83 /* 8 bits, no Parity, 1 Stop bit */			
#define uart0_init() uart_init(0)
#define BIT(X)    ( 1<<X )
#define ENTER 0xd // Ascii value for carriage return i.e enter key
#define command 0x25 // ascii value for the % sign

typedef unsigned int U32;

typedef enum {RUNNING, WAITING_FOR_INTERRUPT} iprocess_state;   

typedef struct interrupt_t{
	// The i-process contains a pcb for interprocess communication
	pcb* pcb;
   iprocess_state state;
} i_process;
     
int uart_init(int n_uart); /* initialize the n_uart, interrupt driven */

/* write a string to the n_uart */
void uart_send_string( uint32_t n_uart, uint8_t *p_buffer, uint32_t len );

extern i_process* interrupt_process;

extern void k_UART0_IRQHandler(void);
 
extern void print_ready_queue_priority(void);
extern void print_memory_blocked_queue_priority(void);
extern void print_message_blocked_queue_priority(void);

extern volatile uint8_t g_UART0_TX_empty=1;
extern volatile uint8_t g_UART0_buffer[BUFSIZE];
extern volatile uint32_t g_UART0_count = 0;
#endif
