/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER_H_
#define _TIMER_H_

extern uint32_t timer_init ( uint8_t n_timer );  /* initialize timer n_timer */

typedef timer{
/*	uint32_t hour;
	uint32_t minute;
	uint32_t seconds;*/

	pcb* pcb;
} timer_iprocess;

extern void k_TIMER0_IRQHandler();
#define TIMER0_IRQHandler() _TIMER0_IRQHandler((U32)k_TIMER0_IRQHandler)
extern void __SVC_0 _TIMER0_IRQHandler(U32 p_func);

#endif /* ! _TIMER_H_ */
