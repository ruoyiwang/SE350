#ifndef __CRT_DISPLAY_PROCESS__
#define __CRT_DISPLAY_PROCESS__

//#include "process.h"
#include "memory.h"
//#include "uart_irq.h";
#include <LPC17xx.h>
#include "interrupt.h"
//#include "iprocesses.c"

extern int display_message_ready;

void crt_displpay_process(){
	int message_length = 0;
	char * message_pointer;
	envelope * message_envelop = NULL;
	message_envelop = receive_message();
	while(message_envelop != NULL){
		//pretty much as long as I get messages, I do this shit
		//will only care if the message is a display request, or else I just delete the message wihtout handling it
		if (message_envelop->type == DISPLAY_REQUEST){
			//handle the message
			//notice here I am actually just changing the msg info and gonna pass it on anyways
			//message_envelop->src_id = current_process->pid;

			//7 is the process id for the i-process
			//message_envelop->dest_id = 7;
			//message_envelop->message_type = "some message type needed";
			//for now use the same request

			//tell the global var that I have a display request in mailbox
			//display_message_ready = 1;

			//send message to i-process that contains the message and shit
			//send_message(message_envelop->dest_id, message_envelop);

			//TODO: skip the hardware interrupt and talk to iprocess directly
			//i_process_routine();
			
				// TODO: get the message length
				message_length = 0;
				message_pointer = message_envelop->message;
				//I'm trying to find EOS here but hacing trouble
				//TODO: debug this shit
				while (*message_pointer != 0){
					message_length++;
					message_pointer++;
				}
				uart_send_string(0, (uint8_t *) message_envelop->message, message_length);

			//unset the message ready
			display_message_ready = 0;
		}

		release_memory_block(message_envelop);
		message_envelop = receive_message();
	}
}

#endif 
