#ifndef __NULL_PROCESS__
#define __NULL_PROCESS__

void null_process() {
	while(true) {
		#ifdef DEBUG_0
		printf("NULL PROCESS");
		#endif
		release_processor();
	}
}

#endif