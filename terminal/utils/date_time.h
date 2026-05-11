#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <unistd.h>

#define SEC_TO_NS(sec) ((sec) * 1000000000)
#define NS_TO_SECF(ns) (float)((ns) / 1000000000.0f)

uint64_t nanosecond_timestamp(void);

#endif
