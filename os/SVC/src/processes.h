#ifndef __PROCESSES__
#define __PROCESSES__

void null_process() {
	while(true) {
		#ifdef DEBUG_0
		printf("NULL PROCESS");
		#endif
		release_processor();
	}
}

void test_process_1() {

}

void test_process_2() {
	
}

void test_process_3() {
	
}

void test_process_4() {
	
}

void test_process_5() {
	
}

void test_process_6() {
	
}

#endif