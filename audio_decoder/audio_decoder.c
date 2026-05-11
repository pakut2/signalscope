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

audio_decoder speaker_audio_decoder_start(on_samples_processed samples_processed_callback) {
    ca_audio_tap *tap = calloc(1, sizeof(*tap));
    assert(tap != NULL);

    ca_loopback_device_start(tap, on_speaker_audio_capture, samples_processed_callback);

    return (audio_decoder){(size_t)tap->format.mSampleRate, {.speaker = tap}};
}

void speaker_audio_decoder_stop(ca_audio_tap *tap) {
    ca_loopback_device_stop(tap);
}

audio_decoder microphone_audio_decoder_start(on_samples_processed samples_processed_callback) {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 0;
    config.dataCallback = on_microphone_audio_capture;
    config.pUserData = samples_processed_callback;

    ma_device *device = calloc(1, sizeof(*device));
    assert(device != NULL);
    ma_result ma_result = ma_device_init(NULL, &config, device);
    assert(ma_result == MA_SUCCESS);

    ma_device_start(device);

    return (audio_decoder){device->sampleRate, {.microphone = device}};
}

void microphone_audio_decoder_stop(ma_device *device) {
    ma_device_uninit(device);
    free(device);
}

audio_decoder audio_decoder_start(DEVICE_TYPE device_type, on_samples_processed samples_processed_callback) {
    switch (device_type) {
        case SPEAKER: {
            return speaker_audio_decoder_start(samples_processed_callback);
        }
        case MICROPHONE: {
            return microphone_audio_decoder_start(samples_processed_callback);
        }
    }
}

void audio_decoder_stop(DEVICE_TYPE device_type, device device) {
    switch (device_type) {
        case SPEAKER: {
            speaker_audio_decoder_stop(device.speaker);

            break;
        }
        case MICROPHONE: {
            microphone_audio_decoder_stop(device.microphone);
        }
    }
}
