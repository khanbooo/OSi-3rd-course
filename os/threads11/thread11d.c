#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

int global = 6;

void *mythread(void *arg) {
    int local = 3;
    static int static_local;
    char* name = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread, my name is %s!\n\n",\
            getpid(), getppid(), gettid(), pthread_self(), name);
    printf("values:\nlocal: %d\n"
           "static_local: %d\n",
           local,
           static_local
    );
    local = 4;
    static_local++;
//    global = 7;
    sleep(10);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t* tids = (pthread_t*)malloc((argc - 2) * sizeof(pthread_t));
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());
    sleep(15);
    for (int i = 0; i < argc - 1; i++){
        err = pthread_create(&tids[i], NULL, mythread, argv[i + 1]);
        printf("%lu\n", tids[i]);
        sleep(10);
    }
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

