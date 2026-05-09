#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

#include <stddef.h>

#define PUBLIC __attribute__((visibility("default")))
#define SAMPLE_COUNT (1 << 13)

typedef struct {
    float *normalized_frequencies;
    size_t frequency_bin_count;
} spectrum;

PUBLIC void spectrum_analyzer_init(void);
PUBLIC void spectrum_samples_append(float *audio_samples, size_t audio_samples_count);
PUBLIC spectrum spectrum_create(size_t sample_rate);
PUBLIC void spectrum_destroy(spectrum *spectrum);

#endif
