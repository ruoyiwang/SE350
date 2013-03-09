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
	kcd_node* tmp = command_root;
	int i = 0;
	char* tmpc = command;

	while (tmp) {
		tmp = tmp->next;
	}

	tmp = (kcd_node*)request_memory_block();
	while (*tmpc && *tmpc != ' ') {
		tmp->command[i] = *tmpc;
		tmpc++;
		i++;
	}
	tmp->command[i] = '\0';
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
			if (tmp->command[i] != *(tmpc+i)) {
				break;
			}
			if (tmp->command[i] == '\0' && *(tmpc+i) == '\0') {
				return tmp->pid;
			}
		} while (tmp->command[i] != '\0' && *(tmpc+i) != '\0');
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
		m = (envelope *)receive_message();
		input = (char*)m->message;
		mt = m->type;
		src_id = m->src_id;

		if (mt == COMMAND_REGISTRATION) {
			if ((*input)== '%') {
				if (*(input + 1)) {
					kcd_register(input + 1, src_id);
				}
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
			// Send to CRT
		}
		release_memory_block((void*)m);
	}
}
