#include "hash.h"
#include <pthread.h>

#define MAX_CAPACITY 1000

typedef struct data_t {
    int value;
} data_t;

typedef struct HashNode {
    char* key;
    data_t data;
    struct HashNode* next;
} HashNode;

typedef struct HashMap {
    uint64_t cur_len;
    pthread_rwlock_t lock;
    HashNode** list_heads;
} HashMap;

void init_hashmap(HashMap* hashmap, uint64_t capacity);
void destroy_hashmap(HashMap* hashmap);
void insert(HashMap* hashmap, const char* key, int value);
void remove(HashMap* hashmap, const char* key);
uint64_t get_hash(const char* key);
data_t* get(HashMap* hashmap, const char* key);
