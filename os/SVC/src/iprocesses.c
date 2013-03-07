void i_process_routine(){
	envelope* kcd_command = k_request_memory_block();
	kcd_command->message = command;
	kcd_command->message_type = COMMAND_REGISTRATION;
	kcd_command->src_id = interrupt_process->pcb->pid;
	kcd_command->dest_id = kcd_process_id;
	// We read from uart so now we send commands to the kcb
	send_message(kcd_command->dest_id, kcd_command);
}