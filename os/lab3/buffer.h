#include <stdio.h>
#include <stdatomic.h>

typedef struct buffer {
    size_t capacity;
    atomic_int ref_count;
    char data[];
} buffer_t;

buffer_t *buffer_create(size_t capacity);

buffer_t *buffer_copy(buffer_t *buffer);

void buffer_destroy(buffer_t *buffer);
