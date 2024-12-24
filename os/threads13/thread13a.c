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
    char* string;
}myStruct;

void *mythread(void *arg) {
    myStruct* innerInstance = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread!\nvalue: = %d\nstring = %s\n",\
            getpid(), getppid(), gettid(), pthread_self(), innerInstance->value, innerInstance->string);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    myStruct* instance = (myStruct*)malloc(sizeof(myStruct));
    instance->value = 12345;
    instance->string = "hello from instance";
//    void ** retval;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, instance);
    if (err) {
        perror("thread create failed");
        return -1;
    }
    if (pthread_join(tid, NULL)){
        perror("thread create failed");
        return -1;
    }
    free(instance);
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

