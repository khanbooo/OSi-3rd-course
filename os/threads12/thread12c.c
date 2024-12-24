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
char* string = "hello world";

void *mythread(void *arg) {
    int local = 3;
    static int static_local = 4;
    const int const_local = 5;
    char* name = arg;
    printf("mythread [%d %d %d %lu]: Hello from mythread, my name is %s!\n\n",\
            getpid(), getppid(), gettid(), pthread_self(), name);
    printf("addresses:\nlocal: %p\n"
           "static local: %p\n"
           "constant local: %p\n"
           "global: %p\n",
           &local,
           &static_local,
           &const_local,
           &global
    );
//    sleep(3);
    return string;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    void ** retval;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, argv[1]);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    if (pthread_join(tid, retval)){
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }
    printf("joined thread returned %s on exit\n", *(char **)retval);
//    printf("%s\n", retval);
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

