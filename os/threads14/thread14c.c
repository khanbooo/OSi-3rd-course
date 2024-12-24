#define _GNU_SOURCE
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static int cleanup_pop_arg = 0;
static char* str;

void cleanup_handler(void* str_to_clear){
    free((char*)str_to_clear);
    printf("Called cleanup_handler and freed memory\n");
}

void *mythread(void *arg) {
    pthread_cleanup_push(cleanup_handler, str);
    str = (char *)malloc(sizeof(char) * strlen("hello world\n\0"));
    char* str_tmp = "hello world\n\0";
    for (int i = 0; i < strlen("hello world\n\0"); i++){
        str[i] = str_tmp[i];
    }

    for(;;){
        pthread_testcancel();
        sleep(1);
        printf("mythread [%d %d %d %lu]: %s",\
            getpid(), getppid(), gettid(), pthread_self(), str);
    }

    pthread_cleanup_pop(cleanup_pop_arg);
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

    cleanup_pop_arg = 1;
    pthread_cancel(tid);

    err = pthread_join(tid, &res);
    if (err != 0) {
        handle_error_en(err, "pthread_join\n");
    }

    if (res == PTHREAD_CANCELED){
        printf("main(): thread was canceled\nstr = %s || ", str);
    }
    else {
        printf("main(): thread wasn't canceled (shouldn't happen!)\n");
    }
//    exit(EXIT_SUCCESS);
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}


