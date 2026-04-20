#include "./utils/date_time.h"
#include "audio_decoder.h"
#include "renderer.h"
#include "spectrum_analyzer.h"

#define ENABLE_CURSES 1

const size_t max_height = 50;
const size_t spacing = 2;
const float smoothness = 8.0f;

float interpolated_frequencies[SAMPLE_COUNT];

// 60 FPS
const float tick_sec = 1.0f / 60.0f;
const uint64_t tick_ns = SEC_TO_NS(1) / 60;

uint64_t start, next_tick;

void on_audio_data(spectrum spectrum) {
    renderer_clear();

    for (size_t i = 0; i < spectrum.size; i++) {
        interpolated_frequencies[i] += (spectrum.normalized_frequencies[i] - interpolated_frequencies[i]) * tick_sec * smoothness;

        render_bar(interpolated_frequencies[i] * max_height, spacing * i);
    }

    renderer_refresh();

    next_tick += tick_ns;
    uint64_t now = nanosecond_timestamp();

    if (now < next_tick) {
        usleep(NS_TO_US(next_tick - now));
    }
}

int main(void) {
#if ENABLE_CURSES
    renderer_init();
#endif

    start = nanosecond_timestamp();
    next_tick = start;

    decode_speaker_audio(on_audio_data);

#if ENABLE_CURSES
    renderer_shutdown();
#endif
}
