#include "spectrum_analyzer.h"
#include "ring_buffer.h"
#include <assert.h>
#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#define MIN_FREQUENCY 35.0f
#define MAX_FREQUENCY 20000.0f
#define MIN_FREQUENCY_LOUDNESS -FLT_MAX
#define SEMITONE_FREQUENCY_COEFFICIENT 1.06f

ring_bufferf samples;
float complex frequencies[SAMPLE_COUNT];

float apply_hanning_window(float period) {
    return 0.5f - 0.5f * cosf(2.0f * (float)M_PI * period);
}

void smoothen_samples(float samples[], float smoothed_samples[], size_t sample_count) {
    for (size_t i = 0; i < sample_count; i++) {
        float period = (float)i / (float)(sample_count - 1);

        smoothed_samples[i] = samples[i] * apply_hanning_window(period);
    }
}

void fft(float samples[], float complex frequencies[], size_t sample_count, size_t step) {
    assert(sample_count > 0 && (sample_count & (sample_count - 1)) == 0); // Array size must be a power of 2

    if (sample_count == 1) {
        frequencies[0] = samples[0];

        return;
    }

    fft(samples, frequencies, sample_count / 2, step * 2);
    fft(samples + step, frequencies + sample_count / 2, sample_count / 2, step * 2);

    for (size_t i = 0; i < sample_count / 2; i++) {
        float period = (float)i / (float)sample_count;

        float complex frequency = cexp(-2 * I * M_PI * period) * frequencies[i + sample_count / 2];
        float complex existing_frequency = frequencies[i];

        frequencies[i] = existing_frequency + frequency;
        frequencies[i + sample_count / 2] = existing_frequency - frequency;
    }
}

float apply_a_weight(float frequency) {
    float ra = (powf(12194.0f, 2) * powf(frequency, 4)) /
               ((powf(frequency, 2) + powf(20.6f, 2)) * (powf(frequency, 2) + powf(12194, 2)) *
                sqrtf((powf(frequency, 2) + powf(107.7f, 2)) * (powf(frequency, 2) + powf(737.9f, 2))));

    return 20.0f * log10f(ra) + 2.0f;
}

float apply_b_weight(float frequency) {
    float rb = (powf(12194.0f, 2.0f) * powf(frequency, 3.0f)) /
               ((powf(frequency, 2.0f) + powf(20.6f, 2.0f)) *
                sqrtf(powf(frequency, 2.0f) + powf(158.5f, 2.0f)) *
                (powf(frequency, 2.0f) + powf(12194.0f, 2.0f)));

    return 20.0f * log10f(rb) + 0.17f;
}

float apply_c_weight(float frequency) {
    float rc = (powf(12194.0f, 2.0f) * powf(frequency, 2.0f)) /
               ((powf(frequency, 2.0f) + powf(20.6f, 2.0f)) *
                (powf(frequency, 2.0f) + powf(12194.0f, 2.0f)));

    return 20.0f * log10f(rc) + 0.06f;
}

float apply_d_weight(float frequency) {
    float h = (powf(1037918.48f - powf(frequency, 2.0f), 2.0f) + 1080768.16f * powf(frequency, 2.0f)) /
              (powf(9837328.0f - powf(frequency, 2.0f), 2.0f) + 11723776.0f * powf(frequency, 2.0f));

    float rd = (frequency / 6.8966888496476e-5f) *
               sqrtf(h / ((powf(frequency, 2.0f) + 79919.29f) * (powf(frequency, 2.0f) + 1345600.0f)));

    return 20.0f * log10f(rd);
}

float find_frequency_loudness(float complex frequency, size_t frequency_count, size_t bin_index, size_t sample_rate) {
    float loudness = 20.0f * log10f(cabsf(frequency));
    float bin_magnitude = (float)bin_index * (float)sample_rate / (float)(frequency_count * 2); // Assume mirrored frequencies are not included

    // TODO experiment with K-weighting
    return loudness + apply_a_weight(bin_magnitude);
}

float get_frequency_bin_index(float frequency, size_t frequency_count, size_t sample_rate) {
    return frequency * (float)(frequency_count * 2) / (float)sample_rate;
}

float find_next_frequency_bin(float previous_bin) {
    return ceilf(previous_bin * SEMITONE_FREQUENCY_COEFFICIENT);
}

float clamp01(float value) {
    return fminf(fmaxf(value, 0.0f), 1.0f);
}

spectrum normalize_frequencies(float complex frequencies[], size_t frequency_count, size_t sample_rate) {
    float max_loudness = MIN_FREQUENCY_LOUDNESS;
    for (size_t i = 1; i < frequency_count; i++) {
        float loudness = find_frequency_loudness(frequencies[i], frequency_count, i, sample_rate);

        if (loudness > max_loudness) {
            max_loudness = loudness;
        }
    }

    if (max_loudness == MIN_FREQUENCY_LOUDNESS) {
        return (spectrum){NULL, 0};
    }

    float min_frequency_bin = get_frequency_bin_index(MIN_FREQUENCY, frequency_count, sample_rate);
    float max_frequency_bin = get_frequency_bin_index(MAX_FREQUENCY, frequency_count, sample_rate);

    size_t frequency_bin_count = 0;
    for (float f = min_frequency_bin; (size_t)f < frequency_count && f < max_frequency_bin; f = find_next_frequency_bin(f)) {
        frequency_bin_count++;
    }

    float *normalized_frequencies = malloc(frequency_bin_count * sizeof normalized_frequencies[0]);
    assert(normalized_frequencies != NULL);

    size_t current_frequency_bin = 0;
    for (float f = min_frequency_bin; (size_t)f < frequency_count && f < max_frequency_bin; f = find_next_frequency_bin(f)) {
        size_t next_frequency_bin = (size_t)find_next_frequency_bin(f);

        float max_semitone_loudness = MIN_FREQUENCY_LOUDNESS;
        for (size_t i = (size_t)f; i < frequency_count && i < next_frequency_bin; i++) {
            float loudness = find_frequency_loudness(frequencies[i], frequency_count, i, sample_rate);

            if (loudness > max_semitone_loudness) {
                max_semitone_loudness = loudness;
            }
        }

        float normalized_frequency_loudness = 0.0f;

        if (max_semitone_loudness > MIN_FREQUENCY_LOUDNESS) {
            float display_range_loudness = 50.0f; // Display top 50dB

            normalized_frequency_loudness = (max_semitone_loudness - (max_loudness - display_range_loudness)) / display_range_loudness;
            normalized_frequency_loudness = powf(clamp01(normalized_frequency_loudness), 5.0f); // Magnify differences

            float silence_loudness_threshold = -5.0f, full_loudness_threshold = 0.0f;
            float silence_gate = (max_loudness - silence_loudness_threshold) / (full_loudness_threshold - silence_loudness_threshold);

            normalized_frequency_loudness *= clamp01(silence_gate);
        }

        normalized_frequencies[current_frequency_bin++] = normalized_frequency_loudness;
    }

    return (spectrum){normalized_frequencies, frequency_bin_count};
}

void spectrum_analyzer_init(void) {
    samples = ring_bufferf_create(SAMPLE_COUNT);
}

void spectrum_samples_append(float *audio_samples, size_t audio_sample_count) {
    ring_bufferf_append(&samples, audio_samples, audio_sample_count);
}

spectrum spectrum_create(size_t sample_rate) {
    float samples_frame[SAMPLE_COUNT];
    ring_bufferf_read(&samples, samples_frame);

    float smoothed_samples[SAMPLE_COUNT];
    smoothen_samples(samples_frame, smoothed_samples, SAMPLE_COUNT);

    fft(smoothed_samples, frequencies, SAMPLE_COUNT, 1);

    return normalize_frequencies(frequencies, SAMPLE_COUNT / 2, sample_rate);
}

void spectrum_destroy(spectrum *spectrum) {
    free(spectrum->normalized_frequencies);
    spectrum->normalized_frequencies = NULL;
}
