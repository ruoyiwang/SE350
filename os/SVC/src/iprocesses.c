#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "memory.h"
#include "uart_polling.h"
#include "interrupt.h"
#include "timer.h"
#include "CRT.h"

int display_message_ready;

void i_process_routine(void){
	char* char_buffer_string;
	// make an empty envelope to for the crt msgs
	envelope* crt_message = NULL;
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;

	// Create an envelope for the kcd message send
	envelope* kcd_command = k_request_memory_block();
	kcd_command->src_id = interrupt_process->pcb->pid;
	kcd_command->dest_id = kcd_process_id;

	// Make sure that interrupts don't add to the char buffer
	// Disable the RBR in the IER
	pUart->IER = pUart->IER & 0xfffffff8;

	// If the first char in the buffer is 0x25 i.e '%', then we send a command registration
	if(g_UART0_buffer[0] == 0x25){
		for(int i = 0 ; i < g_UART0_count; i++){
			*(char+i) = g_UART0_buffer[i];
			g_UART0_buffer[i] = 0;
		}
		g_UART0_count = 0;
		// Code for sending a message to the KCD for a command registration
		kcd_command->type = COMMAND_REGISTRATION;
		kcd_command->message = char_buffer_string;
		send_message(kcd_command->dest_id, kcd_command);
	}	
	else if(display_message_ready == 1){		//if there's a message ready for me to print to CRT
			//above var is the old "roys flag set"
		//receive the message from mail box
		crt_message = receive_message();	//or should it be k_

		//check of message type
		if (message_envelop->message_type != DISPLAY_REQUEST){
			return;
		}
		// TODO: confirm if I should consume the message

		// TODO: get the message
		uart_send_string(0, (uint8_t) message_envelop->message, 0);

		// Code for displaying char to uart0
		uart0_put_string(crt_string);
	}
	//else we know that we send a keyboard input
	else{
		for(int i = 0 ; i < g_UART0_count; i++){
			*(char+i) = g_UART0_buffer[i];
			g_UART0_buffer[i] = 0;
		}
		g_UART0_count = 0;
		// Code for sending a message to the KCD for a command registration
		kcd_command->type = KEYBOARD_INPUT;
		kcd_command->message = char_buffer_string;
		send_message(kcd_command->dest_id, kcd_command);
	}	

	// Since we are done with the interrupt we can reenable RBR
	pUart->IER = pUart->IER | 0x7; 

}

void uart_send_string( uint32_t n_uart, uint8_t *p_buffer, uint32_t len )
{
	LPC_UART_TypeDef *pUart;

	if(n_uart == 0 ) { /* UART0 is implemented */
		pUart = (LPC_UART_TypeDef *)LPC_UART0;
	} else { /* other UARTs are not implemented */
		return;
	}

	if (len == 0){	//special case where I don't know the lenth
		
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
