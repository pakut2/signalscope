#include "./ring_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ring_bufferf ring_bufferf_create(size_t size) {
    ring_bufferf ring_buffer;
    ring_buffer.size = size;
    ring_buffer.buffer = calloc(size, sizeof(float));

    return ring_buffer;
}

void ring_bufferf_append(ring_bufferf *ring_buffer, float items[], size_t items_count) {
    for (size_t i = 0; i < items_count; i++) {
        memmove(ring_buffer->buffer, ring_buffer->buffer + 1, (ring_buffer->size - 1) * sizeof(ring_buffer->buffer[0]));

        ring_buffer->buffer[ring_buffer->size - 1] = items[i];
    }
}
