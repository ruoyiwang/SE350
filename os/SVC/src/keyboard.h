#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef struct kcd_node_t {
	int pid;
	struct kcd_node_t* next;
	char _command[32];
} kcd_node;

void kcd(void);

#endif
