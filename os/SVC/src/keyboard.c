#include "keyboard.h"
#include "memory.h"
#include "process.h"

kcd_node* command_root;

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

void kcd_register(char* command, int pid) {
	kcd_node* tmp;
	int i = 0;
	char* tmpc = command;


	tmp = (kcd_node*)request_memory_block();
	while (*tmpc && *tmpc != ' ') {
		tmp->_command[i] = *tmpc;
		tmpc++;
		i++;
	}
	tmp->_command[i] = '\0';
	tmp->pid = pid;
	tmp->next = command_root;
	command_root = tmp;
}

int kcd_lookup(char* command) {
	kcd_node* tmp = command_root;
	char* tmpc = command;
	int i = 0;

	while (tmp) {
		i = -1;
		do {
			i++;
			if (i > 0 && tmp->_command[i] == '\0' && (*(tmpc+i) == '\0' || *(tmpc+i) == ' ')) {
				return tmp->pid;
			}
			if (tmp->_command[i] != *(tmpc+i)) {
				break;
			}
		} while (tmp->_command[i] != '\0' && (*(tmpc+i) != '\0' || *(tmpc+i) == ' '));
		tmp = tmp->next;
	}

	return -1;
}

void kcd() {
	char* input;
	envelope *m;
	message_type mt;
	int src_id;

	command_root = NULL;

	while (1) {
		m = (envelope *)receive_message(NULL);
		input = (char*)m->message;
		mt = m->type;
		src_id = m->src_id;

		if (mt == COMMAND_REGISTRATION) {
			if (*input) {
					kcd_register(input, src_id);
			}
		}
		else if (mt == KEYBOARD_INPUT) {
			if (*input == '%') {
				// Send message to PID registered to the command
				if (*(input + 1)) {
					src_id = kcd_lookup(input + 1);
					
					if (src_id != -1) {
						m->dest_id = src_id;
						send_message(src_id, m);
					}
				}
			}
			m->type = DISPLAY_REQUEST;
			send_message(8, m);
			
			input = "\n\r";
			m = request_memory_block();
			m->type = DISPLAY_REQUEST;
			m->message = input;
			send_message(8, m);
		}
	}
}
