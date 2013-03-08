#include "interrupt.h"
#ifndef _TIMER_H_
#define _TIMER_H_

extern uint32_t timer_init ( uint8_t n_timer );  /* initialize timer n_timer */

extern i_process* timer;

extern void k_TIMER0_IRQHandler(void);

#endif /* ! _TIMER_H_ */
