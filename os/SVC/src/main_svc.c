/**
 * @brief:  main_svc.c, demonstrate svc as a gateway to os functions
 * @author: Yiqing Huang
 * @date:   2013/01/06
 * NOTE: standard C library is not allowed in the final kernel code
 *       it is OK if you only use the C library for debugging purpose 
 *       during the development/prototyping phase
 */
#ifndef NULL
#define NULL (void *)0
#endif
#include <LPC17xx.h>
#ifdef DEBUG_0
#include <stdio.h>
#endif /* DEBUG_0 */
#include "uart_polling.h"
#include "process.h"
#include "memory.h"

int main()
{
   
  volatile unsigned int ret_val = 1234;

  SystemInit();  /* initialize the system */
  __disable_irq();
  uart1_init();
	mmu_init();
  process_init();
  __enable_irq();

  // transit to unprivileged level, default MSP is used
  __set_CONTROL(__get_CONTROL() | BIT(0));  

  ret_val = release_processor();
  ret_val = release_memory_block(NULL);
  ret_val = (unsigned int) request_memory_block();
#ifdef DEBUG_0
  /* printf has been retargeted to use the UART0,
     check Retarget.c file
  */
  printf("The ret_val=%d\n",ret_val); 
#endif // DEBUG_0
  return 0;  
}
