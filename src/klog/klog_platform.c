#include "./klog_platform.h"

#include <stdlib.h>

#include "klog/klog.h"

#include "./klog_debug_util.h"

procid_t klog_platform_get_current_thread_id(void) {
    return syscall(SYS_gettid);
}

const char* klog_platform_get_basename(const char* const s_filepath) {
    return basename((char*)s_filepath);
}

timepoint_t klog_platform_get_current_timepoint(void) {
    const time_t now = time(NULL);
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        kdprintf("Failure when invoking gettimeofday()\n");
        exit(KLOG_EXIT_CODE);
    }
    const struct tm* const p_tm = localtime(&now);

    timepoint_t timepoint = {
        tv.tv_sec,

        tv.tv_usec,
        p_tm->tm_sec,
        p_tm->tm_min,
        p_tm->tm_hour,
        p_tm->tm_yday + 1,
        p_tm->tm_mon + 1,
        p_tm->tm_year + 1900,

        p_tm->tm_wday + 1,
        p_tm->tm_yday + 1,

        p_tm->tm_isdst
    };

    return timepoint;
}
