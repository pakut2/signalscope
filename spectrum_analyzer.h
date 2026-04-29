#ifndef SPECTRUM_ANALYZER_H
#define SPECTRUM_ANALYZER_H

#include <complex.h>
#include <unistd.h>

typedef struct {
    float *normalized_frequencies;
    size_t frequency_bin_count;
} spectrum;

typedef void (*on_frame_analyzed)(spectrum);

void smoothen_samples(float samples[], float smoothed_samples[], size_t number_of_samples);
void fft(float samples[], float complex frequencies[], size_t n, size_t step);
spectrum normalize_frequencies(float complex frequencies[], size_t frequency_count, size_t sample_rate);
void spectrum_destroy(spectrum *spectrum);

#endif
