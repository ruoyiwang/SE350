#ifndef __PROCESSES_H
#define __PROCESSES_H

#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "uart_polling.h"
#include "memory.h"

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
      uart1_put_string("\n\r");
#endif /* DEBUG_0 */
    }
    uart1_put_char('A' + i%26);
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
      uart1_put_string("\n\r");
#endif  /* DEBUG_0 */
    }
    uart1_put_char('a' + i%26);
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
    int priority;
    int prioritySet;    // priority we want to set on the process
	while(1) {
				prioritySet = 2;
        set_process_priority(4, prioritySet);
        priority = get_process_priority(4);
				uart1_put_string("EXPECTED PRIORITY: 2\nIS PRIORITY 2?");
        if(priority == prioritySet){
            uart1_put_string("YES");
        }
        else{
            uart1_put_string("NO");
        }
				uart1_put_string("IS PRIORITY 0?");
        priority = get_process_priority(4);
        if(priority == 0){
            uart1_put_string("YES");
        }
        else{
            uart1_put_string("NO");
        }

		release_processor();
	}
}

void test_process_5() {
  void * memory;
  int release_success;
	while(1) {
		#ifdef DEBUG_0
      memory = request_memory_block();
      release_success = release_memory_block(memory);
      if (release_success == 0){
        printf("memory test success!\n");
      }
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
