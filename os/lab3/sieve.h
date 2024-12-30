#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include "hashmap.h"
#include "stream.h"

typedef struct cache_entry {
    char *key;
    stream_t *value;
    atomic_bool visited;
    struct cache_entry *next;
    struct cache_entry *prev;
} cache_entry_t;

typedef struct cache {
    int len;
    int capacity;
    hashmap_t hashmap;
    cache_entry_t *head;
    cache_entry_t *tail;
    cache_entry_t *hand;    
    pthread_rwlock_t lock;
} cache_t;

void *cache_init(cache_t *cache, int capacity);

void cache_destroy(cache_t *cache);

void cache_get(cache_t *cache, char *key, stream_t **value);

void cache_put(cache_t *cache, char *key, stream_t *value);

void cache_get_or_put(cache_t *cache, char *key, stream_t **got, stream_t *put);
