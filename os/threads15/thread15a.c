#define _GNU_SOURCE
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define NUM_OF_THREADS 3
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE);} while (0)

static sigset_t mask;

static void handler(int signo) {
    printf("\nsig = %d; tid = %d\n", signo, gettid());
}

void* myThreadWaitSigQuit(void *arg) {
    sigset_t unblock_set;
    sigemptyset(&unblock_set);
    sigaddset(&unblock_set, SIGQUIT);
    pthread_sigmask(SIG_UNBLOCK, &unblock_set, NULL);

    unsigned long err;
    int signo;
    for (;;){
        if((err = sigwait(&unblock_set, &signo) != 0)){
            handle_error_en(err, "sigwait\n");
        }
        if (signo == SIGQUIT){
            handler(signo);
        }
    }
    return NULL;
}

void* myThreadHandleSigint(void *arg) {
    sigset_t unblock_set;
    sigemptyset(&unblock_set);
    sigaddset(&unblock_set, SIGINT);
    pthread_sigmask(SIG_UNBLOCK, &unblock_set, NULL);
    for (;;){}
    return NULL;
}

void* myThreadBlock(void *arg) {
    //blocks everything by default, alternatively, to show that this thread really blocks
    //every signal, we could use sigwait with empty mask
    for (;;){}
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tids[NUM_OF_THREADS];
    void* funcs[] = {myThreadBlock, myThreadHandleSigint, myThreadWaitSigQuit};
    void* res;
    unsigned long err;

    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());

    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_mask = mask;
    act.sa_handler = handler;

    sigaction(SIGINT, &act, 0);

    for (int i = 0; i < NUM_OF_THREADS; i++){
        err = pthread_create(&tids[i], NULL, funcs[i], NULL);
        if (err != 0) {
            handle_error_en(err, "pthread_create\n");
        }
    }

    for (int i = 0; i < NUM_OF_THREADS; i++){
        err = pthread_join(tids[i], &res);
        if (err != 0) {
            handle_error_en(err, "pthread_join\n");
        }
    }
    printf("main thread exits\n");
    pthread_exit(NULL);
    return 0;
}


