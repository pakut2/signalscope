#include "spectrum_analyzer.h"
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>

#define MIN_FREQUENCY 1.0f
#define SEMITONE_FREQUENCY_COEFFICIENT 1.06f

float apply_hanning_window(float period) {
    return 0.5 - 0.5 * cosf(2 * M_PI * period);
}

void smoothen_samples(float samples[], float smoothed_samples[], size_t number_of_samples) {
    for (size_t i = 0; i < number_of_samples; i++) {
        float period = (float)i / (number_of_samples - 1);

        smoothed_samples[i] = samples[i] * apply_hanning_window(period);
    }
}

void fft(float samples[], float complex frequencies[], size_t number_of_samples, size_t step) {
    assert(number_of_samples > 0 && (number_of_samples & (number_of_samples - 1)) == 0); // Array size must be a power of 2

    if (number_of_samples == 1) {
        frequencies[0] = samples[0];

        return;
    }

    fft(samples, frequencies, number_of_samples / 2, step * 2);
    fft(samples + step, frequencies + number_of_samples / 2, number_of_samples / 2, step * 2);

    for (size_t i = 0; i < number_of_samples / 2; i++) {
        float period = (float)i / number_of_samples;

        float complex frequency = cexp(-2 * I * M_PI * period) * frequencies[i + number_of_samples / 2];
        float complex existing_frequency = frequencies[i];

        frequencies[i] = existing_frequency + frequency;
        frequencies[i + number_of_samples / 2] = existing_frequency - frequency;
    }
}

float find_amplitude(float complex frequency) {
    // power scale
    float real = crealf(frequency);
    float imaginary = cimagf(frequency);

    return logf(powf(real, 2) + powf(imaginary, 2));

    // linear scale
    // return cabsf(frequency);
}

// TODO equal loundness weighting
spectrum normalize_frequencies(float complex frequencies[], size_t number_of_frequencies) {
    float max_amplitude = 0.0f;
    for (size_t i = 0; i < number_of_frequencies; i++) {
        float amplitude = find_amplitude(frequencies[i]);

        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
        }
    }

    if (max_amplitude == 0.0f) {
        float *normalized_frequencies = {};

        return (spectrum){normalized_frequencies, 0};
    }

    size_t number_of_frequency_groups = 0;
    for (float f = MIN_FREQUENCY; (size_t)f < number_of_frequencies; f = ceilf(f * SEMITONE_FREQUENCY_COEFFICIENT)) {
        number_of_frequency_groups++;
    }

    float *normalized_frequencies = malloc(number_of_frequency_groups * sizeof normalized_frequencies[0]);

    size_t current_frequency_group = 0;
    for (float f = MIN_FREQUENCY; (size_t)f < number_of_frequencies; f = ceilf(f * SEMITONE_FREQUENCY_COEFFICIENT)) {
        float semitone_frequency = ceilf(f * SEMITONE_FREQUENCY_COEFFICIENT);

        float max_semitone_amplitude = 0.0f;
        for (size_t i = (size_t)f; i < number_of_frequencies && i < (size_t)semitone_frequency; i++) {
            float amplitude = find_amplitude(frequencies[i]);

            if (amplitude > max_semitone_amplitude) {
                max_semitone_amplitude = amplitude;
            }
        }

        float normalized_frequency = max_semitone_amplitude / max_amplitude;

        normalized_frequencies[current_frequency_group++] = normalized_frequency;
    }

    return (spectrum){normalized_frequencies, number_of_frequency_groups};
}

void spectrum_destroy(spectrum *spectrum) {
    free(spectrum->normalized_frequencies);
    spectrum->normalized_frequencies = NULL;
}
