#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "memory.h"
#include "uart_polling.h"
#include "interrupt.h"
#include "timer.h"
#include <LPC17xx.h>
//#include "CRT.h"

int display_message_ready;
volatile uint8_t g_UART0_TX_empty=1;
volatile uint8_t g_UART0_buffer[BUFSIZE];
volatile uint32_t g_UART0_count = 0;

void uart_send_string( uint32_t n_uart, uint8_t *p_buffer, uint32_t len );

void i_process_routine(void){
	uint8_t IIR_IntId;      /* Interrupt ID from IIR */		
	uint8_t LSR_Val;        /* LSR Value             */
	uint8_t dummy = dummy;	/* to clear interrupt upon LSR error */
	envelope* kcd_command;
	char* char_buffer_string;
	// make an empty envelope to for the crt msgs
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	int i = 0;
	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; /* skip pending bit in IIR */

	if (IIR_IntId & IIR_RDA) { /* Receive Data Avaialbe */
		g_UART0_buffer[g_UART0_count] = pUart->RBR;
		++g_UART0_count;
		// Check if the user clicked enter
		if(g_UART0_buffer[g_UART0_count-1] == ENTER){
			// Create an envelope for the kcd message send
			envelope* kcd_command = k_request_memory_block();
			kcd_command->src_id = interrupt_process.pcb.pid;

			// Make sure that interrupts don't add to the char buffer
			// Disable the RBR in the IER
			//pUart->IER = pUart->IER & 0xfffffff8;
			
			for(i = 0 ; i < g_UART0_count; i++){
				*(char_buffer_string+i) = g_UART0_buffer[i];
				g_UART0_buffer[i] = 0;
			}
			g_UART0_count = 0;
			// Code for sending a message to the KCD for a command registration
			// Set the message destination id to the id of the crt process
			kcd_command->dest_id = 7;
			kcd_command->type = KEYBOARD_INPUT;
			kcd_command->message = char_buffer_string;
			k_send_message(kcd_command->dest_id, kcd_command);
		}
		
#ifdef DEBUG_0
		// Check if the hotkeys have been pressed 
		// User presses 1
		if(g_UART0_buffer[g_UART0_count-1] == 0x31){
			print_ready_queue_priority();
		}
		else if(g_UART0_buffer[g_UART0_count-1] == 0x32){
			print_memory_blocked_queue_priority();
		}
		else if(g_UART0_buffer[g_UART0_count-1] == 0x33){
			print_message_blocked_queue_priority();
		}
#endif		
		
		if (g_UART0_count == BUFSIZE) {
			g_UART0_count = 0; /* buffer overflow */
		}
	} else if (IIR_IntId & IIR_THRE) { 
		/* THRE Interrupt, transmit holding register empty*/
		
		LSR_Val = pUart->LSR;
		if(LSR_Val & LSR_THRE) {
			g_UART0_TX_empty = 1; /* ready to transmit */ 
		} else {  
			g_UART0_TX_empty = 0; /* not ready to transmit */
		}
	    
	} else if (IIR_IntId & IIR_RLS) { /* Receive Line Status id = 011 */
		// LSR = Line Status Register. Contains flags for transmit and 
		// receive status, including line errors.
		LSR_Val = pUart->LSR;
		if (LSR_Val  & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
			/* There are errors or break interrupt 
		           Read LSR will clear the interrupt 
			   Dummy read on RX to clear interrupt, then bail out
			*/
			dummy = pUart->RBR; 
			return; /* error occurs, return */
		}
		/* If no error on RLS, normal ready, save into the data buffer.
	           Note: read RBR will clear the interrupt 
		*/
		if (LSR_Val & LSR_RDR) { /* Receive Data Ready */
			/* read from the uart */
			// If user presses enter then we enter the i-process
			g_UART0_buffer[g_UART0_count++] = pUart->RBR; 

			if(g_UART0_buffer[g_UART0_count-1] == ENTER){
				// Create an envelope for the kcd message send
				kcd_command = k_request_memory_block();
				kcd_command->src_id = interrupt_process.pcb.pid;

				// Make sure that interrupts don't add to the char buffer
				// Disable the RBR in the IER
				//pUart->IER = pUart->IER & 0xfffffff8;
				
				for(i = 0 ; i < g_UART0_count; i++){
					*(char_buffer_string+i) = g_UART0_buffer[i];
					g_UART0_buffer[i] = 0;
				}
				g_UART0_count = 0;
				// Code for sending a message to the KCD for a command registration
				// Set the message destination id to the id of the crt process
				kcd_command->dest_id = 7;
				kcd_command->type = KEYBOARD_INPUT;
				kcd_command->message = char_buffer_string;
				k_send_message(kcd_command->dest_id, kcd_command);
			}
			if ( g_UART0_count == BUFSIZE ) {
				g_UART0_count = 0;  /* buffer overflow */
			}
		}	    
	}

	// Since we are done with the interrupt we can reenable RBR
	//pUart->IER = pUart->IER | 0x7; 

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
	envelope* env;
	envelope *temp;
	g_timer_count++;
	if (delay_message_list->front->expire_time > g_timer_count)
	{
		finished_env = delay_message_list->front;
		delay_message_list->front = delay_message_list->front->next;
		finished_env->next = NULL;
		k_send_message(finished_env->dest_id, finished_env);
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
			k_send_message(finished_env->dest_id, finished_env);
		}			
	}
	env = (envelope *) k_request_memory_block();
	env->type = TIMER_UPDATE;
	k_send_message(9, env);
}
