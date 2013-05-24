/*
    mjutil.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjutil.h"
#define _USE_MATH_DEFINES
#ifdef __APPLE__
#   include <mach/mach_time.h>
#else
#   include <time.h>
#endif

// multiplatform timing
long getTimeMillis(void) {
#ifdef __APPLE__
    static mach_timebase_info_data_t timebaseInfo;
    if (timebaseInfo.denom == 0) {
        mach_timebase_info(&timebaseInfo);
    }
    unsigned long long elapsed = mach_absolute_time();
    return elapsed*timebaseInfo.numer/timebaseInfo.denom/1000000;
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec*1000+now.tv_nsec/1000000;
#endif
}
