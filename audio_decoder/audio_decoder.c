#include "audio_decoder.h"
#include "coreaudio/coreaudio_tap.h"
#include "miniaudio/miniaudio.h"
#include <assert.h>

void on_speaker_audio_capture(float *audio_samples, size_t audio_samples_count, void *user_data) {
    on_samples_processed callback = user_data;
    callback(audio_samples, audio_samples_count);
}

void on_microphone_audio_capture(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
    on_samples_processed callback = device->pUserData;
    callback((float *)input, frame_count);
}

void decode_speaker_audio(on_device_init device_init_callback, on_samples_processed samples_processed_callback) {
    ca_audio_tap *tap = calloc(1, sizeof(*tap));
    assert(tap != NULL);

    ca_loopback_device_start(tap, on_speaker_audio_capture, samples_processed_callback);

    device_init_callback((size_t)tap->format.mSampleRate);

    ca_loopback_device_stop(tap);
}

void decode_microphone_audio(on_device_init device_init_callback, on_samples_processed samples_processed_callback) {
    ma_context context;
    ma_result ma_result = ma_context_init(NULL, 0, NULL, &context);
    assert(ma_result == MA_SUCCESS);

    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 0;
    config.dataCallback = on_microphone_audio_capture;
    config.pUserData = samples_processed_callback;

    ma_device device;
    ma_result = ma_device_init(NULL, &config, &device);
    assert(ma_result == MA_SUCCESS);

    ma_device_start(&device);

    device_init_callback(device.sampleRate);

    ma_device_uninit(&device);
}
