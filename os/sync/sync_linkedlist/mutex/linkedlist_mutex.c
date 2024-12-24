#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "linkedlist_mutex.h"

void print_node(LinkedListNode *node) {
    if (node) {
        printf("%s -> ", node->value);
    } else {
        printf("NULL\n");
    }
}

void print_list(LinkedList *list) {
    LinkedListNode *current = list->first;
    while (current) {
        print_node(current);
        current = current->next;
    }
}

void *list_monitor(void *arg) {
    LinkedList *list = (LinkedList *)arg;

    printf("list_monitor: [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        list_print_stats(list);
        // print_list(list);
        sleep(1);
    }

    return NULL;
}

LinkedList* list_init(int capacity) {
    int err;

    LinkedList *list = malloc(sizeof(LinkedList));
    if (!list) {
        printf("Cannot allocate memory for a list\n");
        abort();
    }

    list->first = NULL;
    list->last = NULL;

    list->inc_seq_counter = 0;
    list->dec_seq_counter = 0;
    list->equal_counter = 0;
    list->swap_counter = 0;

    list->capacity = capacity;
    list->length = 0;

    err = pthread_create(&list->list_monitor_tid, NULL, list_monitor, list);
    if (err) {
        printf("list_init: pthread_create() failed: %s\n", strerror(err));
        abort();
    }

    return list;
}   

void list_destroy(LinkedList *list) {
    LinkedListNode *current = list->first;
    LinkedListNode *next;

    while (current) {
        printf("about to free node %p\n", current);
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

int list_add(LinkedList *list, char *value) {
    assert(list->length <= list->capacity);

    if (list->length == list->capacity){
		return 0;
	}

    LinkedListNode *new = malloc(sizeof(LinkedListNode));
    if (!new) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    strcpy(new->value, value);
    new->next = NULL;
    pthread_mutex_init(&new->lock, NULL);

    if (!list->first) {
        list->first = list->last = new;
    } else {
        list->last->next = new;
        list->last = list->last->next;
    }

    list->length++;

    return 1;
}

int list_get(LinkedList *list, char *value) {
    assert(list->length >= 0);

    if (list->length == 0) {
        return 0;
    }

    LinkedListNode *tmp = list->first;

    strcpy(value, tmp->value);
    list->first = list->first->next;

    free(tmp);
    list->length--;

    return 1;
}

void list_print_stats(LinkedList *list) {
    printf(
        "-------------------------------------------------------------------------------------------\n"
        "|list_print_stats:\n [IDS] %d %d %d\n  [COUNTERS] %d %d %d %d\n"
        "-------------------------------------------------------------------------------------------\n", 
    getpid(), getppid(), gettid(), 
    list->inc_seq_counter, list->dec_seq_counter, list->equal_counter, list->swap_counter);
}