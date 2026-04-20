#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <unistd.h>

typedef struct {
    float *buffer;
    size_t size;
} ring_bufferf;

ring_bufferf ring_bufferf_create(size_t size);
void ring_bufferf_append(ring_bufferf *buffer, float items[], size_t items_count);

#endif
