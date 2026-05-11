#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include "coreaudio/coreaudio_tap.h"
#include "miniaudio/miniaudio.h"
#include <stddef.h>

#define PUBLIC __attribute__((visibility("default")))

typedef enum DEVICE_TYPE {
    SPEAKER,
    MICROPHONE
} DEVICE_TYPE;

typedef union {
    ca_audio_tap *speaker;
    ma_device *microphone;
} device;

typedef struct {
    size_t sample_rate;
    device device;
} audio_decoder;

typedef void (*on_samples_processed)(float *samples, size_t sample_count);

PUBLIC audio_decoder audio_decoder_start(DEVICE_TYPE device_type, on_samples_processed samples_processed_callback);
PUBLIC void audio_decoder_stop(DEVICE_TYPE device_type, device device);

#endif
