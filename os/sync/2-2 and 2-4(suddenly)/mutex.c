#include "mutex.h"
#include <assert.h>

int max(int a, int b){
    return a > b ? a : b;
}

static long futex(_Atomic int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void mutex_init(mutex_t *mutex, type_t type) {
    mutex->lock = 0;
    mutex->owner = (pthread_t)NULL;
    mutex->type = type;
}

status_t mutex_lock(mutex_t *mutex) {
    if (pthread_equal(pthread_self(), mutex->owner)){
        if (mutex->type == ERRORCHECK){
            assert(ERROR_STATUS > 0);
            return ERROR_STATUS;
        }
    }
	while (1){
        volatile int zero = 0;
        volatile pthread_t me = pthread_self();
        volatile pthread_t null_owner = (pthread_t)NULL;
        long err;    

		if (atomic_compare_exchange_strong(&mutex->lock, &zero, 1)){
            atomic_compare_exchange_strong(&mutex->owner, &null_owner, me);
			break;
		}

        
        if (mutex->type == RECURSIVE && atomic_compare_exchange_strong(&mutex->owner, &me, me)){
            atomic_fetch_add(&mutex->lock, 1);
            break;
        }

        err = futex(&mutex->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
        if (err == -1 && errno != EAGAIN) {
            printf("futex return with error: %s\n", strerror(errno));
            abort();
        } 
	}
    return SUCCESS_STATUS;
}

status_t mutex_unlock(mutex_t *mutex) {
    volatile pthread_t me = pthread_self();
    if (!atomic_compare_exchange_strong(&mutex->owner, &me, me)){
        assert(ERROR_STATUS > 0);
        return ERROR_STATUS;
    }

	volatile int one = 1; 
	volatile int zero = 0;
    if (atomic_compare_exchange_strong(&mutex->lock, &one, 1)){
        int err;
        atomic_compare_exchange_strong(&mutex->owner, &me, (pthread_t)NULL);
        atomic_compare_exchange_strong(&mutex->lock, &one, 0);

        err = futex(&mutex->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (err == -1) {
            printf("futex return with error: %s\n", strerror(errno));
            abort();
        }
    }
    else if (mutex->type == RECURSIVE && !atomic_compare_exchange_strong(&mutex->lock, &zero, 0)){
        atomic_fetch_sub(&mutex->lock, 1);
    }
    return SUCCESS_STATUS;
}
