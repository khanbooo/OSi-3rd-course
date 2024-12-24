#include "mutex.h"

long futex(_Atomic int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void mutex_init(mutex_t *mutex) {
    mutex->lock = 0;
}

void mutex_lock(mutex_t *lock) {
	while (1){
		int zero = 0;
        long err;

		if (atomic_compare_exchange_strong(&lock->lock, &zero, 1)){
			break;
		}

        err = futex(&lock->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN) {
            printf("futex return with error: %s\n", strerror(errno));
            abort();
        }
	}
}

void mutex_unlock(mutex_t *lock) {
	int one = 1;

	if (atomic_compare_exchange_strong(&lock->lock, &one, 0)){
        int err;

        err = futex(&lock->lock, FUTEX_WAKE, 0, NULL, NULL, 0);
        if (err == -1) {
            printf("futex return with error: %s\n", strerror(errno));
            abort();
        }
    }
}