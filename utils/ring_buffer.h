#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <pthread.h>
#include <stddef.h>

typedef struct {
    float *buffer;
    size_t size;
    pthread_mutex_t mutex;
} ring_bufferf;

ring_bufferf ring_bufferf_create(size_t size);
void ring_bufferf_append(ring_bufferf *buffer, float items[], size_t items_count);
void ring_bufferf_read(ring_bufferf *ring_buffer, float *destination);

#endif
