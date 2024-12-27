#include "hashmap.h"

void init_hashmap(HashMap* hashmap)
{
    hashmap->cur_len = 0;
    hashmap->list = (LinkedList*)malloc(sizeof(LinkedList) * capacity);
    for (uint64_t i = 0; i < capacity; i++) {
        hashmap->list[i].head = NULL;
    }
    pthread_rwlock_init(&hashmap->lock, NULL);
}
