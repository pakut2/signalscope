#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <unistd.h>

#define SEC_TO_NS(sec) ((sec) * 1000000000)
#define NS_TO_US(ns) ((ns) / 1000)

uint64_t nanosecond_timestamp(void);

#endif
