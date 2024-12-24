#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <syscall.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <unistd.h>
#include <errno.h>
#include "linkedlist_rwlock.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

typedef struct pthread_list_args {
    LinkedList *list;
    int (*cmp)(char *, char *);
}list_args;

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

int is_longer(char *a, char *b) {
    return strlen(a) > strlen(b);
}

int is_length_equal(char *a, char *b) {
    return strlen(a) == strlen(b);
}

int is_shorter(char *a, char *b) {
    return strlen(a) < strlen(b);
}

void *length_cmp(void *arg) {
    struct pthread_list_args *args = (struct pthread_list_args *)arg;
    LinkedList *list = args->list;
    int (*cmp)(char *, char *) = args->cmp;

    printf("length_cmp [%d %d %d]\n", getpid(), getppid(), gettid());

    // outer loop to iterate over the list over and over again
    while (1) {
        LinkedListNode *current = list->first;
        LinkedListNode *next;

        // inner loop to compare the lengths of the values of the list and shift the current node
        while (current) {
            // printf("length_cmp [%d %d %d]", getpid(), getppid(), gettid());
            // printf("is about to lock current node %p\n", current);
            pthread_rwlock_rdlock(&current->lock);
            // printf("length_cmp [%d %d %d]", getpid(), getppid(), gettid());
            // printf("is in critical section with current node %p\n", current);
            next = current->next;
            if (next) {
                pthread_rwlock_rdlock(&next->lock);
                if (cmp(current->value, next->value)) {
                    if (cmp == is_longer) {
                        __sync_fetch_and_add(&list->inc_seq_counter, 1);
                    } else if (cmp == is_shorter) {
                        __sync_fetch_and_add(&list->dec_seq_counter, 1);
                    } else {
                        __sync_fetch_and_add(&list->equal_counter, 1);
                    }
                }
                pthread_rwlock_unlock(&next->lock);
            }
            pthread_rwlock_unlock(&current->lock);

            current = next;

            // printf("length_cmp called stats\n");
            // list_print_stats(list);
        }
    }
    return NULL;
}

void *random_swapper(void *arg) {
    LinkedList *list = (LinkedList *)arg;
    
    printf("random_swapper [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        LinkedListNode *current = list->first;
        LinkedListNode *next;
        LinkedListNode *second_next;

        while (current) {
            pthread_rwlock_wrlock(&current->lock);
            next = current->next;
            if (next) {
                pthread_rwlock_wrlock(&next->lock);
                second_next = next->next;
                if (second_next) {
                    if (rand() % 2) {
                        pthread_rwlock_wrlock(&second_next->lock);
                        current->next = second_next;
                        next->next = second_next->next;
                        second_next->next = next;
                        __sync_fetch_and_add(&list->swap_counter, 1);
                        pthread_rwlock_unlock(&second_next->lock);
                    }
                }
                pthread_rwlock_unlock(&next->lock);
            }
            pthread_rwlock_unlock(&current->lock);   
            current = next;
            
        }
    }

    return NULL;
}

void fill_list(LinkedList *list, int n) {
    for (int i = 0; i < n; i++) {
        char value[100];
        sprintf(value, "value-%d", i);
        list_add(list, value);
    }
}



int main() {
	pthread_t tid;
	// int err;

	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

	LinkedList *list = list_init(100);
    // fill_list(list, 100);
    // fill_list(list, 1000);
    // fill_list(list, 10000);
    fill_list(list, 100000);

    // print_list(list);

    list_args *arg_arr = malloc(3 * sizeof(list_args));
    arg_arr[0] = (list_args){list, is_longer};
    arg_arr[1] = (list_args){list, is_shorter};
    arg_arr[2] = (list_args){list, is_length_equal};

    for (int i = 0; i < 3; i++){
        // struct pthread_list_args *args = malloc(sizeof(struct pthread_list_args));
        // pthread_create(&tid, NULL, list_monitor, list);
        pthread_create(&tid, NULL, length_cmp, &arg_arr[i]);
    }

    // sleep(5);

	for (int i = 0; i < 3; i++) {
        // pthread_join(tid, NULL);
        pthread_create(&tid, NULL, random_swapper, list);
    }

	sched_yield();

	// err = pthread_create(&tid_w, NULL, writer, q);
	// if (err) {
	// 	printf("main: pthread_create() failed: %s\n", strerror(err));
	// 	return -1;
	// }

	// TODO: join threads

	pthread_exit(NULL);

	return 0;
}
