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
	__disable_irq();
	// Set status of current process to interrupted
	current_process->state = INTERRUPT;
	saved_process = current_process;

	k_context_switch(&(interrupt_process.pcb));	

	
	
	__enable_irq();

	// We now have to restore context of the current process
	current_process = saved_process;
	k_context_switch (current_process);
}


