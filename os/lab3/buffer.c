#include "buffer.h"

buffer_t *buffer_create(size_t capacity) {
    buffer_t *buffer = malloc(sizeof(buffer_t));
    buffer->capacity = capacity;
    atomic_init(&buffer->ref_count, 1);
    return buffer;
}

buffer_t *buffer_copy(buffer_t *buffer) {
    atomic_fetch_add(&buffer->ref_count, 1);
    return buffer;
}

void buffer_destroy(buffer_t *buffer) {
    if (atomic_fetch_sub(&buffer->ref_count, 1) == 1) {
        free(buffer);
    }
}
