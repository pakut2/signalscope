#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

#include "./utils/ring_buffer.h"
#include <stddef.h>

#define SAMPLE_COUNT (1 << 13)

extern ring_bufferf samples;

typedef struct {
    float *normalized_frequencies;
    size_t frequency_bin_count;
} spectrum;

void spectrum_analyzer_init(void);
spectrum spectrum_create(size_t sample_rate);
void spectrum_destroy(spectrum *spectrum);

#endif
