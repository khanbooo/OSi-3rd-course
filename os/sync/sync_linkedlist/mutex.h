#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/types.h>

typedef struct mutex {
	_Atomic int lock;
} mutex_t;

// long futex(_Atomic int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3);

void mutex_init(mutex_t *lock);
void mutex_lock(mutex_t *lock);
void mutex_unlock(mutex_t *lock);