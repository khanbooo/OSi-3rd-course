#include "stream.h"

stream_t *stream_create(int capacity) {
    stream_t *stream = malloc(sizeof(stream_t));
    stream->finished = false;
    stream->ret_error = false;
    stream->len = 0;
    stream->ref_count = 1;
    stream->buffer = buffer_create(capacity);
    pthread_mutex_init(&stream->lock, NULL);
    pthread_cond_init(&stream->cond, NULL);
    return stream;
}

stream_t *stream_copy(stream_t *stream) {
    atomic_fetch_add(&stream->ref_count, 1);
    return stream;
}

void stream_destroy(stream_t *stream) {
    if (atomic_fetch_sub(&stream->ref_count, 1) == 1) {
        buffer_destroy(stream->buffer);
        free(stream);
    }
}
