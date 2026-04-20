#include "audio_decoder.h"
#include "./utils/ring_buffer.h"
#include "lib/miniaudio.h"
#include "spectrum_analyzer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ring_bufferf samples;
float complex frequencies[SAMPLE_COUNT];

void on_audio_capture(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
    ring_bufferf_append(&samples, (float *)input, frame_count);

    float smoothed_samples[SAMPLE_COUNT];
    smoothen_samples(samples.buffer, smoothed_samples, SAMPLE_COUNT);

    fft(smoothed_samples, frequencies, SAMPLE_COUNT, 1);

    spectrum normalized_frequencies = normalize_frequencies(frequencies, SAMPLE_COUNT / 2);

    on_frame_analyzed callback = device->pUserData;
    callback(normalized_frequencies);

    spectrum_destroy(&normalized_frequencies);
}

void decode_speaker_audio(on_frame_analyzed callback) {
    samples = ring_bufferf_create(SAMPLE_COUNT);

    ma_context context;
    ma_result context_init_result = ma_context_init(NULL, 0, NULL, &context);
    assert(context_init_result == MA_SUCCESS);

    ma_device_info *playback_info;
    ma_uint32 playback_devices_count;
    ma_result get_devices_result = ma_context_get_devices(&context, &playback_info, &playback_devices_count, NULL, NULL);
    assert(get_devices_result == MA_SUCCESS);

    // TODO native CoreAudio tap
    size_t loopback_device_index;
    bool loopback_device_found = false;
    for (size_t i = 0; i < playback_devices_count; i++) {
        if (strcasestr(playback_info[i].name, "BlackHole")) {
            loopback_device_index = i;
            loopback_device_found = true;

            break;
        }
    }

    assert(loopback_device_found == true);

    // TODO dynamic device choice
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.pDeviceID = &playback_info[loopback_device_index].id;
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 0;
    config.dataCallback = on_audio_capture;
    config.pUserData = callback;

    ma_device device;
    ma_result device_init_result = ma_device_init(NULL, &config, &device);
    assert(device_init_result == MA_SUCCESS);

    ma_device_start(&device);

    getchar();

    ma_device_uninit(&device);
}
