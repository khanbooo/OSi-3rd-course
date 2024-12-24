#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>
#include <pthread.h>

typedef enum{
	NORMAL = 0,
	ERRORCHECK = 1,
	RECURSIVE = 2
} type_t;

typedef enum{
	ERROR_STATUS = -1,
	SUCCESS_STATUS = 0
} status_t;

typedef struct mutex {
	_Atomic int lock;
	_Atomic pthread_t owner;
	type_t type;
} mutex_t;

// static long futex(_Atomic int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);

void mutex_init(mutex_t *lock, type_t type);
status_t mutex_lock(mutex_t *lock);
status_t mutex_unlock(mutex_t *lock);