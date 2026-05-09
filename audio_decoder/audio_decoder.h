#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include <stddef.h>

#define PUBLIC __attribute__((visibility("default")))

typedef void (*on_device_init)(size_t sample_rate);
typedef void (*on_samples_processed)(float *samples, size_t sample_count);

PUBLIC void decode_speaker_audio(on_device_init device_init_callback, on_samples_processed samples_processed_callback);
PUBLIC void decode_microphone_audio(on_device_init device_init_callback, on_samples_processed samples_processed_callback);

#endif
