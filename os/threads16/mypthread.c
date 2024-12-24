#define _GNU_SOURCE
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <asm/ldt.h>
#include <sys/syscall.h>
#include <linux/prctl.h>
#include <asm/prctl.h>
// #include <sigaction.h>


#define STACK_SIZE (8 * 1024 * 1024)  // 8MB
#define TOTAL_SIZE (STACK_SIZE + 4096) // 8MB + guard page
#define CLONE_FLAGS (CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND)

#define THREAD_RUNNING    0
#define THREAD_FINISHED   1

#define CLEANUP_JOIN     0
#define CLEANUP_SELF     1
#define CLEANUP_DETACH   2

// enum cleanup_type { 
//     CLEANUP_JOIN,
//     CLEANUP_SELF,
//     CLEANUP_DETACH
// };

volatile int mythread_id = 0;

typedef void *(*start_routine_t)(void *);

typedef struct mythread_t{
    start_routine_t start_routine;
    void *arg;
    int mythread_id;
    int pid;
    void *retval;
    int state;
    int cleanup_type;
    void *stack;
}mythread_t;

mythread_t* threads[50];

void* thread_func(void *arg);
mythread_t* mythread_self();
[[noreturn]] static void mythread_exit(void *retval);
mythread_t* new_mythread(start_routine_t start_routine, void *arg, int cleanup_type);
int thread_startup(void *arg);
int mythread_create(mythread_t **tid, start_routine_t start_routine, void *arg, int cleanup_type);
int mythread_join(mythread_t *thread, void **retval);
// int mythread_cancel(pthread_t * tid);
int mythread_detach(mythread_t *thread);
int mythread_async_cancel(mythread_t *thread);

mythread_t* new_mythread(start_routine_t start_routine, void *arg, int cleanup_type) {
    mythread_t *mythread = (mythread_t *)malloc(sizeof(mythread_t));
    mythread->start_routine = start_routine;
    mythread->arg = arg;
    mythread->mythread_id = mythread_id++;
    mythread->pid = getpid();
    mythread->retval = NULL;
    mythread->state = THREAD_RUNNING;
    mythread->cleanup_type = cleanup_type;
    return mythread;
}

mythread_t* mythread_self(){
    return threads[gettid() - getpid()];
}

[[noreturn]] static void mythread_exit(void *retval) {
    mythread_t *thread = mythread_self();
    thread->retval = retval;
    thread->state = THREAD_FINISHED;

    printf("Finish state is %d\n", thread->state);

    void *stack_base = (thread->stack);
    
    if (thread->cleanup_type == CLEANUP_SELF || 
        thread->cleanup_type == CLEANUP_DETACH) {
        asm volatile("movq %0, %%rdi\n"
                    "movq %1, %%rsi\n"
                    "movq %2, %%rax\n"
                    "syscall\n" // munmap(stack_base, STACK_SIZE)
                    "movq %3, %%rdi\n"
                    "movq %4, %%rax\n"
                    "syscall\n" // exit(status)
                    :
                    : "r"(stack_base),
                    "r"((long)TOTAL_SIZE),
                    "r"((long)SYS_munmap),
                    "r"(retval),
                    "r"((long)SYS_exit)
                    : "rdi", "rsi", "rax");
    }
    __builtin_unreachable();
}

int thread_startup(void *arg) {
    mythread_t *thread = (mythread_t *)arg;
    
    printf("Thread %d: start_routine is about to run\n", thread->mythread_id);
    
    void *ret = thread->start_routine(thread->arg);
    
    printf("Thread %d: start_routine has returned\n", thread->mythread_id);
    
    mythread_exit(ret);
    return 0;
}

int mythread_create(mythread_t **tid, start_routine_t start_routine, void *arg, int cleanup_type) {
    // int new_tid = gettid() - getpid();
    
    mythread_t *thread = new_mythread(start_routine, arg, cleanup_type);
    int new_tid = thread->mythread_id;
    threads[new_tid] = thread;
    printf("Thread %d: create\n", new_tid);
    void *stack_addr;
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);
    
    stack_addr = mmap(NULL, TOTAL_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
    threads[new_tid]->stack = stack_addr;
    
    mprotect((void*)stack_addr + 4096, STACK_SIZE, PROT_READ|PROT_WRITE);
    
    
    int clone_tid = clone(thread_startup, 
                         (void*)(stack_addr + TOTAL_SIZE),
                         CLONE_FLAGS,
                         threads[new_tid]
                         );
    
    *tid = threads[new_tid];
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    // mythread_id++;


    //return success
    return 0;
}

int mythread_join(mythread_t *thread, void **retval) {
    if (thread->cleanup_type == CLEANUP_SELF || thread->cleanup_type == CLEANUP_DETACH) {
        return -1;
    }
    
    while (thread->state != THREAD_FINISHED) {
        sched_yield();
    }
    
    printf("Thread %d: joined\n", thread->mythread_id);
    
    if (retval != NULL) {
        *retval = thread->retval;
    }
    
    munmap(thread->stack, TOTAL_SIZE);
    free(thread);
    
    return 0;
}

int mythread_detach(mythread_t *thread) {
    if (thread == NULL) {
        return -1;
    }
    
    if (thread->state == THREAD_FINISHED) {
        mythread_exit(NULL);
        return 0;
    }
    
    thread->cleanup_type = CLEANUP_DETACH;
    return 0;
}


void signal_handler(int signo) {
    mythread_t *self = mythread_self();
    printf("Thread %d received signal %d\n", self->mythread_id, signo);
    mythread_exit(NULL);
}

int mythread_async_cancel(mythread_t *thread) {
    if (thread == NULL) {
        return -1;
    }
    return syscall(SYS_tgkill, getpid(), thread->pid, SIGUSR1);
}

void *another_thread_func(void *arg){
    printf("Thread another %d: %s\n", mythread_self()->mythread_id, (char *)arg);
    printf("State: %d\n", mythread_self()->state);
    for (;;);
    return NULL;
}

void *thread_func(void *arg){
    sleep(5);
    return NULL;
}

int main() {
    printf("main [%d %d %d]: Hello from main!\n\n", getpid(), getppid(), gettid());
    sleep(10);
    mythread_t * tid;
    mythread_create(&tid, thread_func, "hello", CLEANUP_DETACH);
    
    // mythread_join(tid, NULL);
    sleep(20);

    mythread_t * another_tid;
    // sleep(5);
    mythread_create(&another_tid, thread_func, "hello", CLEANUP_JOIN);
    sleep(10000);
    // mythread_join(another_tid, NULL);
    // printf("first thread state: %d\n", tid->state);
    // mythread_async_cancel(tid, SIGUSR1);
    // printf("first thread state: %d\n", tid->state);
    return 0;
}

