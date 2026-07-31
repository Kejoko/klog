#include <stdio.h>
#include <stdint.h>

#include "klog/klog.h"
#include "../klog_platform.h"

void run(
    const uint32_t num_threads
) {
    const uint32_t num_logs = 50000;

    const KlogFormatInfo format_info = { 4, 20, 0, false, false };
    const KlogAsyncInfo  async_info  = { 100, num_threads };
    const KlogFileInfo   file_info   = { KLOG_LEVEL_TRACE, "klog_sync_vs_async" };

    const KlogAsyncInfo* p_async_info = (num_threads > 0) ? &async_info : NULL;

    klog_initialize(1, format_info, p_async_info, NULL, &file_info, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("name", 4);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);

    const timepoint_t start = klog_platform_get_current_timepoint();
    for (uint32_t idx_message = 0; idx_message < num_logs; ++idx_message) {
        klog_trace(p_handle, "hello");
    }
    const timepoint_t end = klog_platform_get_current_timepoint();

    klog_deinitialize();

    const double start_seconds        = start.second + ((double)start.microsecond / 1000000.0);
    const double end_seconds          = end.second + ((double)end.microsecond / 1000000.0);
    const double duration_sec         = end_seconds - start_seconds;
    const double duration_sec_per_log = duration_sec / num_logs;

    printf(
        "%d logs with %d backing threads took %02.9f seconds (%.9f per log)\n",
        num_logs,
        num_threads,
        duration_sec,
        duration_sec_per_log
    );
}

int main(
    void
) {
    for (uint32_t i = 0; i < 5; ++i) {
        run(i);
    }

    return 0;
}
