#include "klog/klog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_debug_util.h"

#define NUM_LOG_STATEMENTS 100

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(
    void
) {
    KlogFormatInfo  format_info  = { 1, 50, 0, false, false };
    KlogAsyncInfo   async_info   = { 7, 1 };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(4, format_info, &async_info, &console_info, NULL, NULL);

    const KlogLoggerHandle* handle = klog_logger_create("async1", 6);
    klog_logger_level_set(handle, KLOG_LEVEL_TRACE);

    for (uint32_t i = 0; i < NUM_LOG_STATEMENTS; ++i) {
        klog(handle, KLOG_LEVEL_WARN, "Log statement %d", i);
    }

    klog_deinitialize();
    return 0;
}
