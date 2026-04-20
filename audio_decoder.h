#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include "spectrum_analyzer.h"

#define SAMPLE_COUNT (1 << 13)

void decode_speaker_audio(on_frame_analyzed callback);

#endif
