#include "date_time.h"
#include <time.h>
#include <unistd.h>

uint64_t nanosecond_timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

    return SEC_TO_NS((uint64_t)ts.tv_sec) + (uint64_t)ts.tv_nsec;
}
