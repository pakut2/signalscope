#include "audio_decoder.h"
#include "./coreaudio/coreaudio_tap.h"
#include "lib/miniaudio.h"
#include "spectrum_analyzer.h"
#include <assert.h>

void on_audio_capture(float *audio_samples, size_t audio_samples_count) {
    ring_bufferf_append(&samples, audio_samples, audio_samples_count);
}

void on_speaker_audio_capture(float *audio_samples, size_t audio_samples_count) {
    on_audio_capture(audio_samples, audio_samples_count);
}

void on_microphone_audio_capture(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
    on_audio_capture((float *)input, frame_count);
}

void decode_speaker_audio(on_device_init callback) {
    ca_audio_tap *tap = calloc(1, sizeof(*tap));
    assert(tap != NULL);

    ca_loopback_device_start(tap, on_speaker_audio_capture);

    callback((size_t)tap->format.mSampleRate);

    ca_loopback_device_stop(tap);
}

void decode_microphone_audio(on_device_init callback) {
    ma_context context;
    ma_result ma_result = ma_context_init(NULL, 0, NULL, &context);
    assert(ma_result == MA_SUCCESS);

    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 0;
    config.dataCallback = on_microphone_audio_capture;
    config.pUserData = callback;

    ma_device device;
    ma_result = ma_device_init(NULL, &config, &device);
    assert(ma_result == MA_SUCCESS);

    ma_device_start(&device);

    callback(device.sampleRate);

    ma_device_uninit(&device);
}
