#include "sieve.h"

cache_t *cache_create(int capacity) {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->len = 0;
    cache->capacity = capacity;
    cache->hashmap = hashmap_create(capacity);
    cache->head = NULL;
    cache->tail = NULL;
    cache->hand = NULL;
    pthread_rwlock_init(&cache->lock, NULL);
    return cache;
}

