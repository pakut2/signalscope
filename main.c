#include "./utils/date_time.h"
#include "audio_decoder.h"
#include "renderer.h"
#include "spectrum_analyzer.h"
#include <ncurses.h>

#define ENABLE_CURSES 1

const float height = 50.0f;
const size_t spacing = 2;
const float smoothness = 8.0f;

float interpolated_frequencies[SAMPLE_COUNT];

void render_frame(size_t sample_rate, float frame_elapsed_sec) {
    spectrum spectrum = spectrum_create(sample_rate);

    renderer_clear();

    for (size_t i = 0; i < spectrum.frequency_bin_count; i++) {
        // TODO rapid attack / smooth release
        interpolated_frequencies[i] += (spectrum.normalized_frequencies[i] - interpolated_frequencies[i]) * frame_elapsed_sec * smoothness;

        render_bar(interpolated_frequencies[i] * height, i * spacing);
    }

    renderer_refresh();

    spectrum_destroy(&spectrum);
}

void render_frames(size_t sample_rate) {
    timeout(16);

    uint64_t frame_start = nanosecond_timestamp();

    while (getch() == ERR) {
        uint64_t frame_end = nanosecond_timestamp();
        float frame_elapsed_sec = NS_TO_SECF(frame_end - frame_start);

        frame_start = frame_end;

        render_frame(sample_rate, frame_elapsed_sec);
    }
}

int main(void) {
#if ENABLE_CURSES
    renderer_init();
#endif

    spectrum_analyzer_init();

    // TODO dynamic device choice
    decode_speaker_audio(render_frames);
    // decode_microphone_audio(render_frames);

#if ENABLE_CURSES
    renderer_shutdown();
#endif
}
