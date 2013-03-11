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
	MRS r0, MSP
	BL k_UART0_IRQHandler
	POP{r4-r11, pc}
} 

void k_UART0_IRQHandler(uint32_t msp)
{
	__disable_irq();
	// Save the current process
	current_process->sp = msp;
	__set_MSP((uint32_t ) interrupt_process.pcb.sp);
	//saved_process = current_process;
	i_process_routine();
	
	// We now have to restore context of the current process
	__set_MSP((uint32_t ) current_process->sp);
	__enable_irq();
}


