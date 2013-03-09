#include "interrupt.h"
#include <LPC17xx.h>

volatile uint8_t g_UART0_TX_empty=1;
volatile uint8_t g_UART0_buffer[BUFSIZE];
volatile uint32_t g_UART0_count = 0;

pcb* saved_process;

/**
 * @brief: use CMSIS ISR for UART0 IRQ Handler
 * NOTE: This example shows how to save/restore all registers rather than just
 *       those backed up by the exception stack frame. We add extra
 *       push and pop instructions in the assembly routine. 
 *       The actual c_UART0_IRQHandler does the rest of irq handling
 */
__asm void UART0_IRQHandler(void)
{
	PRESERVE8
	IMPORT k_UART0_IRQHandler
	PUSH{r4-r11, lr}
	BL k_UART0_IRQHandler
	POP{r4-r11, pc}
} 

void k_UART0_IRQHandler(void)
{
	uint8_t IIR_IntId;      /* Interrupt ID from IIR */		
	uint8_t LSR_Val;        /* LSR Value             */
	uint8_t dummy = dummy;	/* to clear interrupt upon LSR error */
	LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	
	__disable_irq();
	// Set status of current process to interrupted
	current_process->state = INTERRUPT;
	saved_process = current_process;

	/* Reading IIR automatically acknowledges the interrupt */
	IIR_IntId = (pUart->IIR) >> 1 ; /* skip pending bit in IIR */

	if (IIR_IntId & IIR_RDA) { /* Receive Data Avaialbe */
		/* read UART. Read RBR will clear the interrupt */

		// If user presses enter then we enter the i-process
		if(pUart->RBR == ENTER){
			k_context_switch(&(interrupt_process.pcb));	
		}
		else{
			g_UART0_buffer[g_UART0_count++] = pUart->RBR;
			if (g_UART0_count == BUFSIZE) {
				g_UART0_count = 0; /* buffer overflow */
			}
		}
#ifdef DEBUG_0
		/* Check if the hotkeys have been pressed */
		// User presses 1
		if(pUart->RBR == 0x31){
			print_ready_queue_priority();
		}
		else if(pUart->RBR == 0x32){
			print_memory_blocked_queue_priority();
		}
		else if(pUart->RBR == 0x33){
			print_message_blocked_queue_priority();
		}
#endif


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
			if(pUart->RBR == ENTER){
				k_context_switch(&(interrupt_process.pcb));	
			}
			else{
				g_UART0_buffer[g_UART0_count++] = pUart->RBR; 
				if ( g_UART0_count == BUFSIZE ) {
					g_UART0_count = 0;  /* buffer overflow */
				}
			}
		}	    
	} else { /* IIR_CTI and reserved combination are not implemented */
		return;
	}	
	
	__enable_irq();

	// We now have to restore context of the current process
	current_process = saved_process;
	k_context_switch (current_process);
}


