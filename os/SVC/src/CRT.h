#include "process.h"
#include "memory.h"
#include "uart_irq.c";
#include <LPC17xx.h>
#include "uart_polling.h"

#ifndef __CRT_DISPLAY_PROCESS__
#define __CRT_DISPLAY_PROCESS__

extern int display_message_ready;

void crt_displpay_process(){
	envelope * message_envelop = null;
	message_envelop = receive_message();
	while(message_envelop != null){
		//pretty much as long as I get messages, I do this shit
		//will only care if the message is a display request, or else I just delete the message wihtout handling it
		if (message_envelop->message_type == DISPLAY_REQUEST){
			//handle the message
			//notice here I am actually just changing the msg info and gonna pass it on anyways
			message_envelop->src_id = current_process->pid;
			message_envelop->dest_id = "i-process' pid";
			//message_envelop->message_type = "some message type needed";
			//for now use the same request

			//tell the global var that I have a display request in mailbox
			display_message_ready = 1;

			//send message to i-process that contains the message and shit
			send_message(message_envelop->dest_id, message_envelop);

			//TODO: need to raise a hardware interupt
			UART0_IRQHandler();

			//unset the message ready
			display_message_ready = 0;
		}

		release_memory_block(message_envelop);
		message_envelop = receive_message();
	}
}

#endif 
