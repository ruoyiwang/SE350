/**
 * @file:   main_uart_polling.c
 * @brief:  echo user input from the keyboard by polling the UART
 * @author: Yiqing Huang
 * @date:   2013/01/06
 * NOTE: This is only an example, your final RTX should NOT use polling
 *       Instead UART should be interrupt driven, check UART_irq example
 *       code which will be given to you later during the term.
 */

#include <LPC17xx.h>
#include "uart_polling.h"

int main()
{
  volatile int c = 'D';

  SystemInit();  /* initialize the system */
  
  __disable_irq();
  uart0_init();
  __enable_irq();
  uart0_put_string("RTX>");
  while (1) {
    c = uart0_get_char();
    uart0_put_char(c);
    if (c == '\r') {
      uart0_put_char('\n');  /* add LF if CR is pressed */
      uart0_put_string("RTX>");
    }
  }
}
