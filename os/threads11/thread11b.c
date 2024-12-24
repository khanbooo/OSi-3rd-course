#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>


void *mythread(void *arg) {
    char* name = arg;
    printf("mythread [%d %d %d]: Hello from mythread, my name is %s!\n", getpid(), getppid(), gettid(), name);
    sleep(3);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    int err;

    printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    for (int i = 0; i < argc - 1; i++){
        err = pthread_create(&tid, NULL, mythread, argv[i + 1]);
    }
    sleep(1);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

