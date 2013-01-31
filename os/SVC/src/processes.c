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
      uart1_put_string("\n\rTEST 1: ");
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
      uart1_put_string("\n\rTEST 2: ");
#endif  /* DEBUG_0 */
    }
    uart1_put_char('a' + i%26);
    i++;
  }
}

void test_process_3() {
	int i, priority;
  //set_process_priority(3, 0);
	while(1) {
		/*for(i =0; i<7;i++)
		{
        priority = get_process_priority(i);
				uart1_put_string("\nPROCESS ");
			  uart1_put_char('0'+i);
				uart1_put_string(" PRIORITY: ");
			  uart1_put_char('0'+priority);			 
		}*/
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
				uart1_put_string("\n TEST 4: EXPECTED PRIORITY: 2\n TEST 4: IS PRIORITY 2? ");
        if(priority == prioritySet){
            uart1_put_string("YES");
        }
        else{
            uart1_put_string("NO");
        }
				uart1_put_string("\n TEST 4: IS PRIORITY 0? ");
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
    memory = (void*)request_memory_block();
    release_success = release_memory_block(memory);
    if (release_success == 0){
      uart1_put_string("\nTEST 5: memory test success!\n");
    }
    else{
      uart1_put_string("TEST 5: memory test fail!\n");
    }
		release_processor();
	}
}

void test_process_6() {
	int priority = 3;
	int i = 0;
	set_process_priority(6, priority);
	while(1) {
		if (i == 0) {
      uart1_put_string("\nTEST 6: This should print once.\n");
		}
		else {
			uart1_put_string("\nTEST 6: If this prints, the test fails.");
		}
		release_processor();
	}
}
