#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "klog/klog.h"
#include "../klog_platform.h"
#include "../klog_state.h" /* So we can report the filename to the user and see why things look so strange (negative durations) */

void run(
    const uint32_t num_threads
) {
    klog_platform_sleep_usec(1000);

    const uint32_t num_logs = 50000;

    const KlogFormatInfo format_info = { 4, 20, 0, false, false };
    const KlogAsyncInfo  async_info  = { 100, num_threads };
    const KlogFileInfo   file_info   = { KLOG_LEVEL_TRACE, "klog_sync_vs_async" };

    const KlogAsyncInfo* p_async_info = (num_threads > 0) ? &async_info : NULL;

    klog_initialize(1, format_info, p_async_info, NULL, &file_info, NULL);
    if (g_klog_state.s_filename == NULL) {
        printf("Output filename is NULL for iteration with %d backing threads!\n", num_threads);
        exit(1);
    }
    if (g_klog_state.p_file == NULL) {
        printf("Output file is NULL for iteration with %d backing threads!\n", num_threads);
        exit(1);
    }

    const KlogLoggerHandle* p_handle = klog_logger_create("name", 4);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);

    const timepoint_t start = klog_platform_get_current_timepoint();
    for (uint32_t idx_message = 0; idx_message < num_logs; ++idx_message) {
        klog_trace(p_handle, "hello");
    }
    const timepoint_t end = klog_platform_get_current_timepoint();

    const double start_seconds        = start.second_j2k + ((double)start.microsecond / 1000000.0);
    const double end_seconds          = end.second_j2k + ((double)end.microsecond / 1000000.0);
    const double duration_sec         = end_seconds - start_seconds;
    const double duration_sec_per_log = duration_sec / num_logs;

    printf(
        "%d logs with %d backing threads took %02.9f seconds (%.9f per log) @ %s\n",
        num_logs,
        num_threads,
        duration_sec,
        duration_sec_per_log,
        g_klog_state.s_filename
    );

    if ((duration_sec > 0) && (duration_sec_per_log > 0)) {
        klog_deinitialize();
        return;
    }

    /* For some reason we have a negative duraiton ... why??? */
    printf("  - For some reason we have a negative duration\n");
    printf("  - start point: %f\n", start_seconds);
    printf("    - seconds: %d\n",   start.second_j2k);
    printf("    - micros : %d\n",   start.microsecond);
    printf("    - usec 2 : %f\n",   ((double)start.microsecond / 1000000.0));
    printf("  - end   point: %f\n", end_seconds);
    printf("    - seconds: %d\n",   end.second_j2k);
    printf("    - micros : %d\n",   end.microsecond);
    printf("    - usec 2 : %f\n",   ((double)end.microsecond / 1000000.0));
    if (g_klog_state.s_filename == NULL) {
        printf("  - output filename is NULL!\n");
    } else {
        printf("  - filename   : %s\n", g_klog_state.s_filename);
    }
    if (g_klog_state.p_file == NULL) {
        printf("  - output file is NULL!\n");
    }

    klog_deinitialize();
    exit(1);
}

int main(
    void
) {
    for (uint32_t i = 0; i < 5; ++i) {
        run(i);
    }

    return 0;
}
