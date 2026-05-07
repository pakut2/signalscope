#include "./ring_buffer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ring_bufferf ring_bufferf_create(size_t size) {
    ring_bufferf ring_buffer;
    ring_buffer.size = size;

    ring_buffer.buffer = calloc(size, sizeof(float));
    assert(ring_buffer.buffer != NULL);

    pthread_mutex_init(&ring_buffer.mutex, NULL);

    return ring_buffer;
}

void ring_bufferf_append(ring_bufferf *ring_buffer, float items[], size_t items_count) {
    pthread_mutex_lock(&ring_buffer->mutex);

    for (size_t i = 0; i < items_count; i++) {
        // TODO head/tail index, lock-free?
        memmove(ring_buffer->buffer, ring_buffer->buffer + 1, (ring_buffer->size - 1) * sizeof(ring_buffer->buffer[0]));

        ring_buffer->buffer[ring_buffer->size - 1] = items[i];
    }

    pthread_mutex_unlock(&ring_buffer->mutex);
}

void ring_bufferf_read(ring_bufferf *ring_buffer, float *destination) {
    pthread_mutex_lock(&ring_buffer->mutex);

    memcpy(destination, ring_buffer->buffer, ring_buffer->size * sizeof(ring_buffer->buffer[0]));

    pthread_mutex_unlock(&ring_buffer->mutex);
}
