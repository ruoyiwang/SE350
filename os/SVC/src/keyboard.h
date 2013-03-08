#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef struct kcd_node_t {
	int pid;
	kcd_node* next;
	char command[32];
} kcd_node;

void kcd();

#endif