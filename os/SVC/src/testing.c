#include "memory.h"
#include "process.h"
#include "testing.h"

int test_passed;
int test_failed;
const int TEST_TOTAL = 6;
int test_init_complete = 0;
char test_string1[] = "\n\rG029_test: START";
char test_string2[] = "\n\rG029_test: total 6 tests";
char test_string3[] = "\n\rG029_test: test n OK";
char test_string4[] = "\n\rG029_test: test m FAIL";
char test_string5[] = "\n\rG029_test: a/6 tests OK";
char test_string6[] = "\n\rG029_test: a/6 tests FAIL";
char test_string7[] = "\n\rG029_test: END";

// Run in kernel mode
void test_init(int n) {
	envelope *ts1 = (envelope*)k_request_kernel_memory_block();
	envelope *ts2 = (envelope*)k_request_kernel_memory_block();

	ts1->dest_id = ts2->dest_id = CRT_PID;
	ts1->type = ts2->type = DISPLAY_REQUEST;
	ts1->message = (void *)&test_string1;
	ts2->message = (void *)&test_string2;
	ts1->next = ts2->next = NULL;
	k_send_message(CRT_PID, ts1);
	k_send_message(CRT_PID, ts2);
	test_passed = 0;
	test_failed = 0;
	test_init_complete = 1;
}

void test_pass() {
	envelope *ts3 = (envelope*)request_test_harness_memory_block();

	if (test_init_complete) {
		test_passed = test_passed + 1;

		test_string3[18] = current_process->pid + '0';
		ts3->src_id = current_process->pid;
		ts3->dest_id = CRT_PID;
		ts3->type = DISPLAY_REQUEST;
		ts3->message = (void *)&test_string3;
		ts3->next = NULL;
		send_message(CRT_PID, ts3);

		if (test_passed + test_failed == TEST_TOTAL) {
			test_finish();
		}
	}
	else {
		release_memory_block(ts3);
	}
}

void test_fail() {
	envelope *ts4 = (envelope*)request_test_harness_memory_block();

	if (test_init_complete) {
		test_failed = test_failed + 1;

		test_string4[18] = current_process->pid + '0';
		ts4->src_id = current_process->pid;
		ts4->dest_id = CRT_PID;
		ts4->type = DISPLAY_REQUEST;
		ts4->message = (void *)&test_string4;
		ts4->next = NULL;
		send_message(CRT_PID, ts4);

		if (test_passed + test_failed == TEST_TOTAL) {
			test_finish();
		}
	}
	else {
		release_memory_block(ts4);
	}
}

void test_finish() {
	envelope *ts5 = (envelope*)request_memory_block();
	envelope *ts6 = (envelope*)request_memory_block();
	envelope *ts7 = (envelope*)request_memory_block();

	if (test_init_complete) {
		test_string5[13] = test_passed + '0';
		test_string6[13] = test_failed + '0';

		ts5->src_id = ts6->src_id = current_process->pid;
		ts5->dest_id = ts6->dest_id = ts7->dest_id = CRT_PID;
		ts5->type = ts6->type = ts7->type = DISPLAY_REQUEST;
		ts5->next = ts6->next = ts7->next = NULL;
		ts5->message = (void *)&test_string5;
		ts6->message = (void *)&test_string6;
		ts7->message = (void *)&test_string7;

		send_message(CRT_PID, ts5);
		send_message(CRT_PID, ts6);
		send_message(CRT_PID, ts7);
	}
	else {
		release_memory_block(ts5);
		release_memory_block(ts6);
		release_memory_block(ts7);
	}
}
