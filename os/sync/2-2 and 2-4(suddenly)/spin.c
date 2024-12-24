#include "spin.h"

void spinlock_init(spinlock_t *spin) {
	spin->lock = 0;
}

void spinlock_lock(spinlock_t *lock) {
	volatile int zero = 0;
	while (1){
		if (atomic_compare_exchange_weak(&lock->lock, &zero, 1)){
			break;
		}
		
	}
}

void spinlock_unlock(spinlock_t *lock) {
	int one = 1;

	atomic_compare_exchange_strong(&lock->lock, &one, 0);
}