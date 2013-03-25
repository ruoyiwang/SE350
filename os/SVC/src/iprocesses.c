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
	char char_buffer_string[BUFSIZE];
	// make an empty envelope to for the crt msgs
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	int i = 0;

	/** Hotkey variables **/
	envelope* hotkey_message;
	pcb* pcb;
	const int hotkey_string_size = 84;
	char hotkey_string[hotkey_string_size];
	int ctr = 0 ;
	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; /* skip pending bit in IIR */

	if (IIR_IntId & IIR_RDA) { /* Receive Data Avaialbe */
		g_UART0_buffer[g_UART0_count] = pUart->RBR;
		++g_UART0_count;
		// Check if the user clicked enter
		if(g_UART0_buffer[g_UART0_count-1] == ENTER){
			// Create an envelope for the kcd message send
			kcd_command = k_request_kernel_memory_block();
			kcd_command->src_id = interrupt_process.pcb.pid;
			
			// Copy the char buffer into a char array to pass into the message
			for(i = 0 ; i < g_UART0_count; i++){
				char_buffer_string[i] = g_UART0_buffer[i];
				g_UART0_buffer[i] = 0;
			}
			// Since kcd is expecting the end of the command string to have \0, we replace the enter with a null char
			char_buffer_string[--i] = '\0';
			g_UART0_count = 0;
			// Code for sending a message to the KCD for a command registration
			// Set the message destination id to the id of the crt process
			kcd_command->dest_id = 10;
			kcd_command->type = KEYBOARD_INPUT;
			kcd_command->message = char_buffer_string;
			k_send_message(kcd_command->dest_id, kcd_command);
		}
		

		// Check if the hotkeys have been pressed 
		// User types exclamation mark
		else if(g_UART0_buffer[g_UART0_count-1] == 0x21){
			--g_UART0_count;
			//hotkey_message = k_request_kernel_memory_block();
			// Traverse through each priority queue in the ready queue
			for(i = 0; i < NUM_PROCS; i ++){
				pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
				if(pcb->state == READY){
					if(pcb->pid > 9){
						uart0_put_char('0' + (pcb->pid/10));
						uart0_put_char('0' + (pcb->pid - (pcb->pid/10)*10));
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
					else{
						uart0_put_char('0' + pcb->pid);
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
				}
			}
			/*hotkey_string[ctr++] = '\0';
			// Send to CRT
			hotkey_message->dest_id = CRT_PID;
			hotkey_message->type = DISPLAY_REQUEST;
			hotkey_message->message = hotkey_string;
			k_send_message(CRT_PID, hotkey_message);*/
		}
		// User types @
		else if(g_UART0_buffer[g_UART0_count-1] == 0x40){
			--g_UART0_count;
			//hotkey_message = k_request_kernel_memory_block();
			for(i = 0; i < NUM_PROCS; i ++){
				pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
				if(pcb->state == MEMORY_BLOCK){
					if(pcb->pid > 9){
						uart0_put_char('0' + (pcb->pid/10));
						uart0_put_char('0' + (pcb->pid - (pcb->pid/10)*10));
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
					else{
						uart0_put_char('0' + pcb->pid);
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
				}
			}
			/*hotkey_string[ctr++] = '\0';
			// Send to CRT
			hotkey_message->dest_id = CRT_PID;
			hotkey_message->type = DISPLAY_REQUEST;
			hotkey_message->message = hotkey_string;
			k_send_message(CRT_PID, hotkey_message);*/
		}
		// User types #
		else if(g_UART0_buffer[g_UART0_count-1] == 0x23){
			--g_UART0_count;
			//hotkey_message = k_request_kernel_memory_block();
			// Traverse through each priority queue in the ready queue
			for(i = 0; i < NUM_PROCS; i ++){
				pcb = pcb_lookup_by_pid(i,pcb_lookup_list);
				if(pcb->state == MESSAGE_BLOCK){
					if(pcb->pid > 9){
						uart0_put_char('0' + (pcb->pid/10));
						uart0_put_char('0' + (pcb->pid - (pcb->pid/10)*10));
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
					else{
						uart0_put_char('0' + pcb->pid);
						uart0_put_char(' ');
						uart0_put_char('0' + pcb->priority);
						uart0_put_char('\n');
						uart0_put_char('\r');
					}
				}
			}
			/*hotkey_string[ctr++] = '\0';
			// Send to CRT
			hotkey_message->dest_id = CRT_PID;
			hotkey_message->type = DISPLAY_REQUEST;
			hotkey_message->message = hotkey_string;
			k_send_message(CRT_PID, hotkey_message);*/
		}
		
		
		if (g_UART0_count == BUFSIZE-1) {
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
				kcd_command = k_request_kernel_memory_block();
				kcd_command->src_id = interrupt_process.pcb.pid;

				// Make sure that interrupts don't add to the char buffer
				// Disable the RBR in the IER
				//pUart->IER = pUart->IER & 0xfffffff8;
				
				for(i = 0 ; i < g_UART0_count; i++){
					char_buffer_string[i] = g_UART0_buffer[i];
					g_UART0_buffer[i] = 0;
				}
				// Since kcd is expecting the end of the command string to have \0, we replace the enter with a null char
				char_buffer_string[--i] = '\0';
				g_UART0_count = 0;
				// Code for sending a message to the KCD for a command registration
				// Set the message destination id to the id of the crt process
				kcd_command->dest_id = 10;
				kcd_command->type = KEYBOARD_INPUT;
				kcd_command->message = char_buffer_string;
				k_send_message(kcd_command->dest_id, kcd_command);
			}
			if ( g_UART0_count == BUFSIZE-1 ) {
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
	if (delay_message_list->front->expire_time < g_timer_count)
	{
		finished_env = delay_message_list->front;
		//finished_env->next = NULL;
		k_send_message(finished_env->dest_id, finished_env);
		if (g_timer_count%1000 == 0)
		{
			env = (envelope *) k_request_kernel_memory_block();
			env->type = TIMER_UPDATE;
			k_send_message(WALLCLOCK_PID, env);
		}
		return;
	}
	temp = delay_message_list->front;
	if (temp!=NULL)
	{
		while ( temp->next != NULL)
		{
			if (temp->next->expire_time < g_timer_count)
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
			temp = temp->next;	
		}
	}
	if (g_timer_count%1000 == 0)
	{
		env = (envelope *) k_request_kernel_memory_block();
		env->type = TIMER_UPDATE;
		k_send_message(WALLCLOCK_PID, env);
	}
}
