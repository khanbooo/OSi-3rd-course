#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdatomic.h>
#include "buffer.h"


typedef struct stream {
    bool finished;
    bool ret_error;
    int len;
    atomic_int ref_count;
    buffer_t *buffer;

    pthread_mutex_t lock;
    pthread_cond_t cond;
} stream_t;

stream_t *stream_create(int capacity);

stream_t *stream_copy(stream_t *stream);

void stream_destroy(stream_t *stream);

