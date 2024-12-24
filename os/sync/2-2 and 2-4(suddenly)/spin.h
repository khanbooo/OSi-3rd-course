#include <stdatomic.h>
#include <stdio.h>

typedef struct spinlock {
	_Atomic int lock;
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_lock(spinlock_t *lock);
void spinlock_unlock(spinlock_t *lock);