#include "klog/klog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../klog_debug_util.h"

#define NUM_LOG_STATEMENTS 50

int main(
    void
) {
    KlogFormatInfo  format_info  = { 7, 50, 0, false, false };
    KlogAsyncInfo   async_info   = { 3, 1, false, false };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    KlogFileInfo    file_info    = { KLOG_LEVEL_TRACE, "klog_async_both" };
    klog_initialize(4, format_info, &async_info, &console_info, &file_info, NULL);

    const KlogLoggerHandle* handle = klog_logger_create("general", 7);
    klog_logger_level_set(handle, KLOG_LEVEL_TRACE);

    for (uint32_t i = 0; i < NUM_LOG_STATEMENTS; ++i) {
        klog(handle, KLOG_LEVEL_WARN, "Log statement %d", i);
    }

    klog_deinitialize();
    return 0;
}
