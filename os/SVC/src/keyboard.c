#include "keyboard.h"
#include "memory.h"

kcd_node* command_root;

int atoi(void* input) {
	char c;
	int n = -1;
	int i = 0;

	while (c = (char)*(input + i)) {
		if (c < '0' || c > '9') {
			return n;
		}
		n = n*10;
		n = n + (c - '0');
		i++;
	}

	return n;
}

void kcd_register(char* command, int pid) {
	kcd_node* tmp = command_root;
	int i = 0;
	char* tmpc = command;

	while (tmp) {
		tmp = tmp->next;
	}

	tmp = (kcd_node*)request_memory_block();
	while (tmpc && tmpc != ' ') {
		tmpc->command[i] = *tmpc;
		tmpc++;
		i++;
	}
	tmp->pid = pid;
}

int kcd_lookup(char* command) {
	kcd_node* tmp = command_root;
	char* tmpc = command;
	int i = 0;

	while (tmp) {
		i = -1;
		do {
			i++;
			if (tmp->command[i] != *(command+i)) {
				break;
			}
			if (tmp->command[i] == '\0' && *(command+i) == '\0') {
				return tmp->pid;
			}
		} while (tmp->command[i] != '\0' && *(command+i) != '\0');
		tmp = tmp->next;
	}

	return -1;
}

void kcd() {
	void* input;
	MessageEnvelope *m;
	message_type mt;
	int src_id;
	char c;
	key_command kc;

	command_root = NULL;

	while (1) {
		m = (MessageEnvelope *)receive_message();
		input = m->message;
		mt = m->type;
		src_id = m->src_id;

		if (mt == COMMAND_REGISTRATION) {
			if ((char)*(input) == '%') {
				if ((char *)(input + 1)) {
					kcd_register((char)*(input + 1), src_id);
				}
			}
		}
		else if (mt == KEYBOARD_INPUT) {
			if ((char)*(input) == '%') {
				// Send message to PID registered to the command
				if ((char)*(input + 1)) {
					src_id = kcd_lookup((char)*(input + 1));
					if (src_id != -1) {
						send_message(input, src_id);
					}
				}
			}
			// Send to CRT
		}
	}
}