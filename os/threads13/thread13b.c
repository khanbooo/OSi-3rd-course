#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

typedef struct myStruct {
    int value;
    char* str;
}myStruct;

void *myThreadAnother(void *arg) {
    sleep(5);
    myStruct* innerInstance = (myStruct*) arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread!\nvalue: = %d\nstring = %s\n",\
            getpid(), getppid(), gettid(), pthread_self(), innerInstance->value, innerInstance->str);
    return NULL;
}

void *mythread(void *arg) {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    unsigned long err;
    myStruct* innerInstance = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread!\nvalue: = %d\nstring = %s\n",\
            getpid(), getppid(), gettid(), pthread_self(), innerInstance->value, innerInstance->str);

    myStruct* instance = (myStruct*)malloc(sizeof(myStruct));
    instance->value = 12345;
    instance->str = "Hello from inner instance\n";
    err = pthread_create(&tid, &attr, myThreadAnother, innerInstance);
    if (err) {
        perror("inner thread create failed");
        return (void*)-1;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    myStruct* instance = (myStruct*)malloc(sizeof(myStruct));
    instance->value = 12345;
    instance->str = "hello from instance\n";
//    myStruct instance = {12345, "hello from instance\n"};
//    void ** retval;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, &attr, mythread, instance);
    if (err) {
        perror("thread create failed");
        return -1;
    }
    pthread_attr_destroy(&attr);
    printf("main thread exits\n");
//    free(instance);
    pthread_exit(NULL);
    return 0;
}

