#ifdef DEBUG_0
#include <stdio.h>
#endif

#include "process.h"
#include "memory.h"
#include "keyboard.h"
#include "uart_polling.h"
#include "testing.h"

int num_passes;         // Holds number of test cases that passed
int num_fails;          // Hold number of test cases that failed
int KCD_PID = 7;
int WALLCLOCK_PID = 9;
int PRIORITY_CHANGE_PID = 11;
int CRT_PID = 8;

/*int atoi(void* input) {
  char c;
  int n = -1;
  int i = 0;
  
  c = (char)(*(input + i));
  while (c) {
    if (c < '0' || c > '9') {
      return n;
    }
    n = n*10;
    n = n + (c - '0');
    i++;
    c = = (char)*(input + i);
  }

  return n;
}*/

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
      //uart0_put_string("\n\rTEST 1: ");
#endif /* DEBUG_0 */
    }
    //uart0_put_char('A' + i%26);
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
      //uart0_put_string("\n\rTEST 2: ");
#endif  /* DEBUG_0 */
    }
    //uart0_put_char('a' + i%26);
    i++;
  }
}

void test_process_3() {
	int i;
	int priority;
  //set_process_priority(3, 0);
	while(1){
		/*for(i =0; i<9;i++)
		{
        priority = get_process_priority(i);
				if(priority== -1){
          test_fail();
          release_processor();
        }
		}*/
    test_pass();
		release_processor();
	}
}

void test_process_4() {
  int priority;
  int prioritySet;    // priority we want to set on the process

	while (1)
	{
    prioritySet = 2;
    set_process_priority(4, prioritySet);
    priority = get_process_priority(4);

    if(priority != prioritySet){
        test_fail();
        release_processor();
    }

    test_pass();
    release_processor();
	}
}

void test_process_5() {
  void * memory;
  int release_success;
	while(1)
	{
		memory = (void*)request_memory_block();
		release_success = release_memory_block(memory);
		if (release_success != 0){
			//uart0_put_string("\n\rG029_test: test 5 FAIL");
			release_processor();
		}
		else{
			//uart0_put_string("\n\rG029_test: test 5 OK");
			release_processor();
		}
	}
}

void test_process_6() {
  char* message ="10 seconds have passed!\n\r";
	envelope * re;
	envelope * crt;
  //envelope* crt_message = NULL;
  while(1) {
		re = (envelope *) request_memory_block();
		re->src_id = 6;
		re->dest_id = 6;
		delay_send(6, re, 10000);
    re = (envelope*)receive_message(NULL);
		
	//	env = (envelope *) k_request_memory_block();
	//	env->type = KEYBOARD_INPUT;
	//	env->message = message;
	//	k_send_message(9, env);
		crt = (envelope*)request_memory_block();
		crt->type = DISPLAY_REQUEST;
		crt->message = message;
		send_message(8, crt);
		
		release_memory_block(re);
	}
}

void wall_clock() {
  envelope e[3];
	envelope* crt;
  envelope* re;
  char* input;
  char wr_message[] = "WR";
  char ws_message[] = "WS";
  char wt_message[] = "WT";
  char time_string[11];
  int hour = 0, minute = 0, second = 0;
  int second_overflow = 0, minute_overflow = 0;
  int on = 1;
	int i;

  e[0].message = (void*)&wr_message;
  e[1].message = (void*)&ws_message;
  e[2].message = (void*)&wt_message;

  // Register for commands WR, WS, WT
  for (i = 0; i < 3; i++) {
    e[i].src_id = WALLCLOCK_PID;
    e[i].dest_id = KCD_PID;
    e[i].type = COMMAND_REGISTRATION;
    e[i].message_length = 2;
    e[i].next = NULL;
    send_message(KCD_PID, &e[i]);
  }

  while (1) {
    re = (envelope*)receive_message(NULL);
    input = (char *)re->message;
    if (on && re->type == TIMER_UPDATE) {
      if (second + 1 >= 60) {
        second_overflow = 1;
      }
      second = (second + 1)%60;

      if (minute + second_overflow >= 60) {
        minute_overflow = 1;
      }
      minute = (minute + second_overflow)%60;
      hour = (hour + minute_overflow)%24;

      second_overflow = minute_overflow = 0;
    }
    else if (re->type != TIMER_UPDATE && *input == '%' && *(input+1) == 'W') {
      if (*(input+2) == 'R') {
        // Reset clock to 00:00:00
        hour = 0;
        minute = 0;
        second = 0;
        on = 1;
      }
      else if (*(input+2) == 'S') {
        // Set clock to HH:MM:SS
        if (*(input+4) >= '0' && *(input+4) <= '2' && *(input+5) >= '0' && *(input+5) <= '9' &&
            *(input+7) >= '0' && *(input+7) <= '5' && *(input+8) >= '0' && *(input+8) <= '9' &&
            *(input+10) >= '0' && *(input+10) <= '5' && *(input+11) >= '0' && *(input+11) <= '9') {
          
          if ((*(input+4) == '2' && *(input+5) > '4') || (*(input+7) == '5' && *(input+8) > '9') || (*(input+10) == '5' && *(input+11) > '9')) {
            continue;
          }        

          hour = 10*(*(input+4) - '0');
          hour = hour + (*(input+5) - '0');
          minute = 10*(*(input+7) - '0');
          minute = minute + (*(input+8) - '0');
          second = 10*(*(input+10) - '0');
          second = second + (*(input+11) - '0');
          on = 1;
        }
      }
      else if (*(input+2) == 'T') {
        // Terminate wallclock
        on = 0;
      }
    }
    if (on) {
      time_string[0] = hour/10 + '0';
      time_string[1] = hour%10 + '0';
      time_string[2] = time_string[5] = ':';
      time_string[3] = minute/10 + '0';
      time_string[4] = minute%10 + '0';
      time_string[6] = second/10 + '0';
      time_string[7] = second%10 + '0';
			time_string[8] = '\n';
			time_string[9] = '\r';
			time_string[10] = '\0';
			crt = (envelope*)request_memory_block();
			crt->src_id = 9;
			crt->dest_id = 8;
			crt->type = DISPLAY_REQUEST;
			crt->message = time_string;
			send_message(8, crt);
    }
		release_memory_block(re);
  }
}

void test_process_a(){
  //void * p = request_memory_block();  //don't think Ineed this line
  
  char c_message[] = "Z";
  char* c_temp;
  int num = 0;
  envelope * p;
  envelope * e = (envelope *)request_memory_block();
  //below registers the Z command
  e->src_id = test_process_a_id;
  e->dest_id = KCD_PID;
  e->type = COMMAND_REGISTRATION;
  e->message = (void*)&c_message;
  e->message_length = 2;
  e->next = NULL;
  send_message(KCD_PID, e);

  //below is the actual test
  while(1){
    p = (envelope*)receive_message(NULL);
    c_temp = p->message;
    if (c_temp[0] == 'Z'){
      release_memory_block(p);
      break;
    }
    else{
      release_memory_block(p);
    }
  }

  while (1){
    p = (envelope*)request_memory_block();
    p->type = COUNT_REPORT;
    c_temp = p->message;
    c_temp[0] = num;
    p->src_id = test_process_a_id;
    p->dest_id = test_process_b_id;
    send_message(p->dest_id, p);
    num = num + 1;
    release_processor();
  }
}

void test_process_b(void){
  envelope * re;
  while(1){
    re = (envelope *) request_memory_block();
    re = (envelope*)receive_message(NULL);

    // After receiving message we send to process C
    /** TODO: Change the src and dest ids **/
    re->src_id = test_process_b_id;
    re->dest_id = test_process_c_id;

    send_message(re->dest_id, re);
  }
}

void test_process_c(void){
  /** perform any needed initialization and create a local message queue **/
  message_queue mqueue;
  envelope *p;
  envelope *q;
  mqueue.front = NULL;
  mqueue.end = NULL;
  while(1){
    // if (local message queue is empty) then
    if(mqueue.front == mqueue.end){
      p = (envelope*)receive_message(NULL);
    }
    else{
      p = (envelope*)mqueue_dequeue(&mqueue);
    }
    if(p->type == COUNT_REPORT){
      if(((uint8_t)(p->message))%20 == 0){
        p->dest_id = CRT_PID;
        p->src_id = test_process_c_id;
        send_message(p->dest_id,p);

        /** Hibernate for 10 seconds **/
        q = (envelope*)request_memory_block();
        q->type = WAKE_UP_10;
        delay_send(test_process_c_id, q, 10000);
        while(1){
          p = receive_message(NULL);
          if(p->type == WAKE_UP_10){
            break;
          }
          else{
            mqueue_enqueue(&mqueue,p);          
					}
        }
      }
    }
    release_memory_block(p);
    release_processor();
  }
}

void priority_change() {
  envelope e;
  envelope* re;
  char* input;
  char c_message[] = "C";
  char time_string[11];
  int pid;
  int priority;

  // Register for command C (usage: %C PID PRIORITY)
  e.src_id = PRIORITY_CHANGE_PID;
  e.dest_id = KCD_PID;
  e.type = COMMAND_REGISTRATION;
  e.message = (void*)&c_message;
  e.message_length = 2;
  e.next = NULL;
  send_message(KCD_PID, &e);

  while (1) {
    re = (envelope*)receive_message(NULL);
    input = (char *)re->message;

    // Currently only accepts 2 digit pids
    if (re->type == KEYBOARD_INPUT) {
      pid = *(input + 3) - '0';
      if (pid < 0 || pid > 9) {
        continue;
      }
      if (*(input + 4) >= '0' && *(input + 4) <= '9') {
        pid = pid*10;
        pid = pid + *(input + 4) - '0';
        priority = *(input + 6) - '0';
      }
      else {
        priority = *(input + 5) - '0';
      }

      set_process_priority(pid, priority);
    }
  }
}
