#include "interrupt.h"
#include <LPC17xx.h>

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
	MRS r0, MSP
	PUSH{r4-r11, lr}
	BL k_UART0_IRQHandler
	POP{r4-r11, pc}
} 

void k_UART0_IRQHandler(uint32_t msp)
{
	current_process->sp = (uint32_t *) msp;
	__disable_irq();
	// Save the current process
	//saved_process = current_process;
	i_process_routine();
	
	// We now have to restore context of the current process
	//__set_MSP((uint32_t ) current_process->sp);
	__enable_irq();
}


