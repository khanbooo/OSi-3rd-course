#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct LinkedListNode {
	char value[100];
	struct Node* next;
} LinkedListNode;	

typedef struct LinkedList {
	LinkedListNode *first;
	LinkedListNode *last;

	pthread_t list_monitor_tid;

	int inc_seq_counter;
	int dec_seq_counter;
	int equal_counter;
	int swap_counter;

	int capacity;
	int length;
	
} LinkedList;

LinkedList* list_init();

void list_destroy(LinkedList *list);
int list_add(LinkedList *list, char *value);
int list_get(LinkedList *list, char *value);
void list_print_stats(LinkedList *list);




#endif		// __FITOS_QUEUE_H__
