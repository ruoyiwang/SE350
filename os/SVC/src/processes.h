#ifndef __PROCESSES_H
#define __PROCESSES_H

#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "uart_polling.h"

void null_process() {
	while(1) {
		#ifdef DEBUG_0
		printf("NULL PROCESS");
		#endif
		release_processor();
	}
}

void test_process_1() {
  volatile int i =0;
  volatile int ret_val = 10;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG_0
      printf("\n\rproc1: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif /* DEBUG_0 */
    }
    uart0_put_char('A' + i%26);
    i++;
  }
}

void test_process_2() {
  volatile int i =0;
  volatile int ret_val = 20;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG_0
      printf("\n\rproc2: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif  /* DEBUG_0 */
    }
    uart0_put_char('a' + i%26);
    i++;
  }
}

void test_process_3() {
	while(1) {
		#ifdef DEBUG_0
		printf("TEST PROCESS 3");
		#endif
		release_processor();
	}	
}

void test_process_4() {
	while(1) {
		#ifdef DEBUG_0
		printf("TEST PROCESS 4");
		#endif
		release_processor();
	}	
}

void test_process_5() {
	while(1) {
		#ifdef DEBUG_0
		printf("TEST PROCESS 5");
		#endif
		release_processor();
	}	
}

void test_process_6() {
	while(1) {
		#ifdef DEBUG_0
		printf("TEST PROCESS 5");
		#endif
		release_processor();
	}	
}

#endif
