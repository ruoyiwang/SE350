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

void uart_send_string( uint32_t n_uart, uint8_t *p_buffer, uint32_t len );

void i_process_routine(void){
	char* char_buffer_string;
	// make an empty envelope to for the crt msgs
	envelope* crt_message = NULL;
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	int message_length = 0;
	void * message_pointer;
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
		crt_message = receive_message();

		//check of message type
		while (crt_message->type != DISPLAY_REQUEST){
			//if it's not a display request, silently kill it
			release_memory_block(crt_message);
			crt_message = receive_message();
		}

		// TODO: get the message length
		message_length = 0;
		message_pointer = crt_message->message;
		while (message_pointer != '\0'){
			message_length++;
		}
		uart_send_string(0, (uint8_t *) crt_message->message, message_length);

		// Code for displaying char to uart0
		//uart0_put_string(crt_message->message);
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
	envelope* finished_env;
	envelope *temp;
	g_timer_count++;
	if (delay_message_list->front->expire_time > g_timer_count)
	{
		finished_env = delay_message_list->front;
		delay_message_list->front = delay_message_list->front->next;
		finished_env->next = NULL;
		send_message(finished_env->dest_id, finished_env);
	}
	temp = delay_message_list->front;
	while ( temp->next != NULL)
	{
		if (temp->next->expire_time > g_timer_count)
		{
			if (temp->next == delay_message_list->end)
			{
				delay_message_list->end = temp;
			}
			finished_env = temp->next;
			temp->next = temp->next->next;
			finished_env->next = NULL;
			send_message(finished_env->dest_id, finished_env);
		}			
	}
}
