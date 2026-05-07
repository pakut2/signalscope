#ifndef COREAUDIO_TAP_H
#define COREAUDIO_TAP_H

#include <CoreAudio/CoreAudio.h>

typedef struct ca_tap_description ca_tap_description;

ca_tap_description *ca_tap_description_create(void);
void ca_tap_description_destroy(ca_tap_description *tap);

OSStatus ca_process_tap_create(ca_tap_description *tap, AudioObjectID *tap_id);
OSStatus ca_process_tap_destroy(AudioObjectID tap);
CFStringRef ca_get_tap_id(ca_tap_description *tap);

typedef void (*on_samples_processed)(float *samples, size_t sample_count);

typedef struct {
    AudioObjectID tap_id;
    AudioObjectID device_id;
    AudioDeviceIOProcID io_proc_id;
    AudioStreamBasicDescription format;
    ca_tap_description *tap_description;
    on_samples_processed callback;
} ca_audio_tap;

void ca_loopback_device_start(ca_audio_tap *tap, on_samples_processed callback);
void ca_loopback_device_stop(ca_audio_tap *tap);

#endif
