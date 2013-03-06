#ifndef __CRT_DISPLAY_PROCESS__
#define __CRT_DISPLAY_PROCESS__
void crt_displpay_process(){
	env * message_envelop = null;
	message_envelop = receive_message();
	while(message_envelop != null){
		//pretty much as long as I get messages, I do this shit
		
		//handle the message

		//NOTE: below is a fake call that's not yet implementated
		i_process_send(message);

		release_memory_block(message_envelop);
		message_envelop = receive_message();		
	}
}


#endif 