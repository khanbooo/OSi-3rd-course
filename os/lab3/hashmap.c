#include "hashmap.h"

void init_hashmap(HashMap* hashmap) {
    hashmap->cur_amount = 0;
    pthread_rwlock_init(&hashmap->lock, NULL);
    hashmap->list_heads = (HashNode**)malloc(MAX_CAPACITY * sizeof(HashNode*));
    for (int i = 0; i < MAX_CAPACITY; i++) {
        hashmap->list_heads[i] = NULL;
    }
}

void destroy_hashmap(HashMap* hashmap) {
    // pthread_rwlock_wrlock(&hashmap->lock);
    for (int i = 0; i < MAX_CAPACITY; i++) {
        HashNode* cur = hashmap->list_heads[i];
        while (cur != NULL) {
            HashNode* next = cur->next;
            free(cur->key);
            // free(cur->data)
            free(cur);
            cur = next;
        }
    }
    free(hashmap->list_heads);
    // pthread_rwlock_unlock(&hashmap->lock);
    // pthread_rwlock_destroy(&hashmap->lock);
}

void insert(HashMap* hashmap, const char* key, int value) {
    // pthread_rwlock_wrlock(&hashmap->lock);
    uint64_t hash = get_hash(key);
    HashNode* cur = hashmap->list_heads[hash];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            cur->data.value = value;
            // pthread_rwlock_unlock(&hashmap->lock);
            return;
        }
        cur = cur->next;
    }
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->key = (char*)malloc(strlen(key) + 1);
    strcpy(new_node->key, key);
    new_node->data.value = value;
    new_node->next = hashmap->list_heads[hash];
    hashmap->list_heads[hash] = new_node;
    hashmap->cur_amount++;
    // pthread_rwlock_unlock(&hashmap->lock);
}

void remove(HashMap* hashmap, const char* key) {
    // pthread_rwlock_wrlock(&hashmap->lock);
    uint64_t hash = get_hash(key);
    HashNode* cur = hashmap->list_heads[hash];
    HashNode* prev = NULL;
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            if (prev == NULL) {
                hashmap->list_heads[hash] = cur->next;
            } else {
                prev->next = cur->next;
            }
            free(cur->key);
            // free(cur->data)
            free(cur);
            hashmap->cur_amount--;
            // pthread_rwlock_unlock(&hashmap->lock);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    // pthread_rwlock_unlock(&hashmap->lock);
}
