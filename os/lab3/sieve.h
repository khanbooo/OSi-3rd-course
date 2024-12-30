#include <stdio.h>
#include <pthread.h>
#include "hashmap.h"
#include "stream.h"

typedef struct cache_entry {
    char *key;
    stream_t *value;
    struct cache_entry *next;
} cache_entry_t;

typedef struct cache {
    int len;
    int capacity;
    HashMap hashmap;
    cache_entry_t *head;
    cache_entry_t *tail;
    cache_entry_t *hand;    
    pthread_rwlock_t lock;
} cache_t;

cache_t *cache_create(int capacity);

void cache_destroy(cache_t *cache);

stream_t *cache_get(cache_t *cache, const char *key);

void cache_put(cache_t *cache, const char *key, stream_t *value);

void cache_get_or_put(cache_t *cache, const char *key, stream_t *value);
