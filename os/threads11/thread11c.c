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
    sleep(10);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t* tids = (pthread_t*)malloc((argc - 1) * sizeof(pthread_t));
//    pthread_t* tid;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());
//    sleep(25);

    for (int i = 0; i < argc - 1; i++){
        err = pthread_create(&tids[i], NULL, mythread, argv[i + 1]);
        printf("%lu\n", tids[i]);
        sleep(5);
//        if (i == 0){
//            tid = (pthread_t *) tids[0];
//        }
    }
//    printf("%d, %d\n", tids[0] == tid, pthread_equal(tids[0], tids[1]));

//    sleep(15);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

