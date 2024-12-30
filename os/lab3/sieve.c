#include "sieve.h"

cache_t *cache_create(int capacity) {
    cache_t *cache = malloc(sizeof(cache_t));
    cache->len = 0;
    cache->capacity = capacity;
    cache->head = NULL;
    cache->tail = NULL;
    cache->hand = NULL;
    hashmap_init(&cache->hashmap);
    pthread_rwlock_init(&cache->lock, NULL);
    return cache;
}

void cache_destroy(cache_t *cache) {
    pthread_rwlock_wrlock(&cache->lock);
    cache_entry_t *entry = cache->head;
    while (entry) {
        cache_entry_t *next = entry->next;
        free(entry->key);
        stream_destroy(entry->value);
        free(entry);
        entry = next;
    }
    hashmap_destroy(&cache->hashmap);
    pthread_rwlock_unlock(&cache->lock);
    pthread_rwlock_destroy(&cache->lock);
    free(cache);
}

void cache_get(cache_t *cache, char *key, stream_t **value) {
    pthread_rwlock_rdlock(&cache->lock);
    cache_entry_t *cache_entry;
    hashmap_get(&cache->hashmap, key, (void **)&cache_entry);

    if (cache_entry) {
        atomic_store(&cache_entry->visited, true);
        *value = stream_copy(cache_entry->value);
    } else {
        *value = NULL;
    }

    pthread_rwlock_unlock(&cache->lock);
}

static void cache_evict(cache_t *cache) {
    cache_entry_t *current = cache->hand;
    
    while (1) {
        if (!atomic_load(&current->visited)){
            if (current == cache->head){
                current->prev->next = NULL;
                cache->head = current->next;
                cache->hand = cache->tail;
            }
            else if (current == cache->tail){
                current->next->prev = NULL;
                cache->tail = current->prev;
                cache->hand = cache->tail;
            }
            else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                cache->hand = current->next;
            }

            break;
        }
        else {
            atomic_store(&current->visited, false);
        }

        if (current->next){
            current = current->next;
        }
        else{
            current = cache->tail;
        }
    }

    free(current->key);
    stream_destroy(current->value);
    free(current);
    cache->len -= 1;
}

static void cache_put_nonfull(cache_t *cache, char *key, stream_t *value){
    cache_entry_t *entry = malloc(sizeof(cache_entry_t));
    entry->key = strdup(key);
    entry->value = stream_copy(value);
    atomic_init(&entry->visited, false);
    entry->next = NULL;
    entry->prev = NULL;

    if (cache->len == 0){
        cache->head = entry;
        cache->tail = entry;
        cache->hand = entry;
    }
    else {
        cache->tail->next = entry;
        entry->prev = cache->tail;
        cache->tail = entry;
    }

    hashmap_insert(&cache->hashmap, entry->key, entry);
    cache->len += 1;
}

void cache_put(cache_t *cache, char *key, stream_t *value) {
    pthread_rwlock_wrlock(&cache->lock);
    if (cache->len == cache->capacity){
        cache_evict(cache);
    }
    cache_put_nonfull(cache, key, value);
    pthread_rwlock_unlock(&cache->lock);
}

void cache_get_or_put(cache_t *cache, char *key, stream_t **got, stream_t *put) {
    pthread_rwlock_wrlock(&cache->lock);
    cache_entry_t *cache_entry;
    hashmap_get(&cache->hashmap, key, (void **)&cache_entry);

    if (cache_entry) {
        atomic_store(&cache_entry->visited, true);
        *got = stream_copy(cache_entry->value);
    } else {
        *got = NULL;
        if (cache->len == cache->capacity){
            cache_evict(cache);
        }
        cache_put_nonfull(cache, key, stream_copy(put));
    }

    pthread_rwlock_unlock(&cache->lock);
}
