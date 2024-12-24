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
	struct LinkedListNode* next;
    pthread_spinlock_t lock;
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
void print_list(LinkedList *list);
void *list_monitor(void *arg);
void print_node(LinkedListNode *node);




#endif		// __FITOS_QUEUE_H__
