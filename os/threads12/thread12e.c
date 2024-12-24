#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

void *mythread(void *arg) {
    unsigned long err = pthread_detach(pthread_self());
    if (err) {
        printf("mythread [%d %d %d %lu]: \n"
               "pthread_detach() failed: %s\n\n", getpid(), getppid(), gettid(), pthread_self(), strerror(err));
        return NULL;
    }
    char* name = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread, my name is %s!\n\n",\
            getpid(), getppid(), gettid(), pthread_self(), name);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    for (;;){
        err = pthread_create(&tid, NULL, mythread, argv[1]);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

