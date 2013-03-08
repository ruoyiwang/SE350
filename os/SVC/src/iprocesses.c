#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "memory.h"
#include "uart_polling.h"
#include "interrupt.h"

void i_process_routine(void){
	// Create an envelope for the kcd message send
	envelope* kcd_command = k_request_memory_block();
	kcd_command->message = g_UART0_buffer[0];
	kcd_command->src_id = interrupt_process->pcb->pid;
	kcd_command->dest_id = kcd_process_id;

	// If the first char in the buffer is 0x25 i.e '%', then we send a command registration
	if(g_UART0_buffer[0] == 0x25){
		// Code for sending a message to the KCD for a command registration
		kcd_command->message_type = COMMAND_REGISTRATION;
		send_message(kcd_command->dest_id, kcd_command);
	}
	else if(roys flag set){
		// Code for displaying char to uart0
		uart0_put_string(crt_string);
	}
	//else we know that we send a keyboard input
	else{
		// Code for sending a message to the KCD for a command registration
		kcd_command->message_type = KEYBOARD_INPUT;
		send_message(kcd_command->dest_id, kcd_command);
	}	
}

void uart_send_string( uint32_t n_uart, uint8_t *p_buffer, uint32_t len )
{
	LPC_UART_TypeDef *pUart;

	if(n_uart == 0 ) { /* UART0 is implemented */
		pUart = (LPC_UART_TypeDef *)LPC_UART0;
	} else { /* other UARTs are not implemented */
		return;
	}

	while ( len != 0 ) {
		/* THRE status, contain valid data  */
		while ( !(g_UART0_TX_empty & 0x01) );	
		pUart->THR = *p_buffer;
		g_UART0_TX_empty = 0;  // not empty in the THR until it shifts out
		p_buffer++;
		len--;
	}
	return;
}
// increment the timer
void timer_iprocess(void){
  g_timer_count++ ;
}