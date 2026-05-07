#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include "stddef.h"

typedef void (*on_device_init)(size_t sample_rate);

void decode_speaker_audio(on_device_init callback);
void decode_microphone_audio(on_device_init callback);

#endif
