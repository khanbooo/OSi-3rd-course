#include "spin.h"

void spinlock_init(spinlock_t *spin) {
	spin->lock = 0;
}

void spinlock_lock(spinlock_t *lock) {
	while (1){
		int zero = 0;

		if (atomic_compare_exchange_strong(&lock->lock, &zero, 1)){
			break;
		}
		
	}
}

void spinlock_unlock(spinlock_t *lock) {
	int one = 1;

	atomic_compare_exchange_strong(&lock->lock, &one, 0);
}