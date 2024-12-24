#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void *mythread(void *arg) {
    int counter = 0;
    for(;;){
        pthread_testcancel();
        counter++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    void *res;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, mythread, NULL);
    if (err) {
        perror("thread create failed\n");
        return -1;
    }
    sleep(5);
    printf("main is about to cancel another thread\n");
    sleep(1);
    pthread_cancel(tid);
    err = pthread_join(tid, &res);
    if (err != 0)
        handle_error_en(err, "pthread_join\n");

    if (res == PTHREAD_CANCELED)
        printf("main(): thread was canceled\n");
    else
        printf("main(): thread wasn't canceled (shouldn't happen!)\n");
//    exit(EXIT_SUCCESS);
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}

