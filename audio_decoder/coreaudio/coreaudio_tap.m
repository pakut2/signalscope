#import "coreaudio_tap.h"
#import <CoreAudio/AudioHardwareTapping.h>
#import <CoreAudio/CATapDescription.h>

struct ca_tap_description {
    CATapDescription *description;
};

ca_tap_description *ca_tap_description_create(void) {
    CATapDescription *tap_description = [[CATapDescription alloc] initMonoGlobalTapButExcludeProcesses:@[]];
    assert(tap_description != nil);

    [tap_description setName:@"SignalScope"];
    [tap_description setPrivate:YES];
    [tap_description setMuteBehavior:CATapUnmuted];

    ca_tap_description *tap = calloc(1, sizeof(*tap));
    assert(tap != NULL);

    tap->description = tap_description;

    return tap;
}

void ca_tap_description_destroy(ca_tap_description *tap) {
    [tap->description release];
    free(tap);
}

OSStatus ca_process_tap_create(ca_tap_description *tap, AudioObjectID *tap_id) {
    return AudioHardwareCreateProcessTap(tap->description, tap_id);
}

OSStatus ca_process_tap_destroy(AudioObjectID tap) {
    return AudioHardwareDestroyProcessTap(tap);
}

CFStringRef ca_get_tap_id(ca_tap_description *tap) {
    NSString *tap_id = [[tap->description UUID] UUIDString];

    return CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)tap_id);
}
