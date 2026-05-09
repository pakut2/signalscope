#include "coreaudio_tap.h"

void ca_get_tap_format(AudioObjectID tap_id, AudioStreamBasicDescription *format) {
    AudioObjectPropertyAddress tap_address = {
        .mSelector = kAudioTapPropertyFormat,
        .mScope = kAudioObjectPropertyScopeGlobal,
        .mElement = kAudioObjectPropertyElementMain
    };

    UInt32 format_size = sizeof(*format);

    OSStatus status = AudioObjectGetPropertyData(
        tap_id,
        &tap_address,
        0,
        NULL,
        &format_size,
        format
    );

    assert(status == noErr);
}

void ca_device_create(CFStringRef tap_uuid, AudioObjectID *device) {
    CFStringRef device_name = CFSTR("SignalScope Loopback device");

    CFUUIDRef device_uuid = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef device_uid = CFUUIDCreateString(kCFAllocatorDefault, device_uuid);
    CFRelease(device_uuid);

    const void *tap_keys[] = {
        CFSTR(kAudioSubTapUIDKey),
        CFSTR(kAudioSubTapDriftCompensationKey)
    };

    const void *tap_values[] = {
        tap_uuid,
        kCFBooleanTrue
    };

    CFDictionaryRef sub_tap = CFDictionaryCreate(
        kCFAllocatorDefault,
        tap_keys,
        tap_values,
        2,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks
    );

    const void *tap_list_values[] = {sub_tap};

    CFArrayRef tap_list = CFArrayCreate(
        kCFAllocatorDefault,
        tap_list_values,
        1,
        &kCFTypeArrayCallBacks
    );

    const void *device_keys[] = {
        CFSTR(kAudioAggregateDeviceNameKey),
        CFSTR(kAudioAggregateDeviceUIDKey),
        CFSTR(kAudioAggregateDeviceTapListKey),
        CFSTR(kAudioAggregateDeviceTapAutoStartKey),
        CFSTR(kAudioAggregateDeviceIsPrivateKey)
    };

    const void *device_values[] = {
        device_name,
        device_uid,
        tap_list,
        kCFBooleanTrue,
        kCFBooleanTrue
    };

    CFDictionaryRef device_description = CFDictionaryCreate(
        kCFAllocatorDefault,
        device_keys,
        device_values,
        5,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks
    );

    OSStatus status = AudioHardwareCreateAggregateDevice(
        device_description,
        device
    );

    CFRelease(device_description);
    CFRelease(tap_list);
    CFRelease(sub_tap);
    CFRelease(device_uid);

    assert(status == noErr);
}

OSStatus ca_tap_callback(
    AudioObjectID in_device,
    const AudioTimeStamp *in_now,
    const AudioBufferList *in_input_data,
    const AudioTimeStamp *in_input_time,
    AudioBufferList *out_output_data,
    const AudioTimeStamp *in_output_time,
    void *in_client_data
) {
    if (in_input_data == NULL || in_input_data->mNumberBuffers == 0) {
        return noErr;
    }

    const AudioBuffer *samples = &in_input_data->mBuffers[0];
    if (samples->mData == NULL || samples->mDataByteSize == 0) {
        return noErr;
    }

    const UInt32 channels = samples->mNumberChannels;
    assert(channels == 1);

    const UInt32 sample_count = samples->mDataByteSize / sizeof(float);

    ca_audio_tap *tap = in_client_data;
    tap->callback(samples->mData, sample_count, tap->user_data);

    return noErr;
}

void ca_loopback_device_start(ca_audio_tap *tap, ca_on_samples_processed callback, void *user_data) {
    tap->callback = callback;
    tap->user_data = user_data;
    tap->tap_description = ca_tap_description_create();

    OSStatus status = ca_process_tap_create(tap->tap_description, &tap->tap_id);
    assert(status == noErr);

    ca_get_tap_format(tap->tap_id, &tap->format);

    CFStringRef tap_id = ca_get_tap_id(tap->tap_description);
    ca_device_create(tap_id, &tap->device_id);
    CFRelease(tap_id);

    status = AudioDeviceCreateIOProcID(
        tap->device_id,
        ca_tap_callback,
        tap,
        &tap->io_proc_id
    );
    assert(status == noErr);

    status = AudioDeviceStart(tap->device_id, tap->io_proc_id);
    assert(status == noErr);
}

void ca_loopback_device_stop(ca_audio_tap *tap) {
    AudioDeviceStop(tap->device_id, tap->io_proc_id);
    AudioDeviceDestroyIOProcID(tap->device_id, tap->io_proc_id);
    AudioHardwareDestroyAggregateDevice(tap->device_id);

    ca_process_tap_destroy(tap->tap_id);
    ca_tap_description_destroy(tap->tap_description);

    free(tap);
    tap = NULL;
}
