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
    char* name = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread, my name is %s!\n\n",\
            getpid(), getppid(), gettid(), pthread_self(), name);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
//    void ** retval;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    for (;;){
        err = pthread_create(&tid, &attr, mythread, argv[1]);
        if (err) {
            perror("thread create failed");
            return -1;
        }
    }
    pthread_attr_destroy(&attr);
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

