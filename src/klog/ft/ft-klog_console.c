/**
 * @brief Ensure that we can log to the console (color and non-color) AND NOT to a file correctly
 */

#include "klog/klog.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void do_it(const bool use_color) {
    KlogFormatInfo format_info = {4, 10, 5, true, false};
    KlogConsoleInfo stdout_info = {KLOG_LEVEL_TRACE, use_color};
    klog_initialize(2, format_info, NULL, &stdout_info, NULL);

    const KlogLoggerHandle* handle_1 = klog_logger_create("Logger");
    klog_logger_level_set(handle_1, KLOG_LEVEL_TRACE);
    klog(handle_1, KLOG_LEVEL_INFO, "0123456789AAAAAAAA");
    klog(handle_1, KLOG_LEVEL_DEBUG, "Debug logging");
    klog(handle_1, KLOG_LEVEL_INFO, "format %d %f stuff %s", 42, 42.42f, "fourty two");

    const KlogLoggerHandle* handle_2 = klog_logger_create("B");
    klog_logger_level_set(handle_2, 6);
    klog(handle_2, KLOG_LEVEL_TRACE, "What's up - trace level - second log statement");

    for (uint32_t i = 0; i < 2; ++i) {
        const KlogLoggerHandle* p_handle = i % 2 ? handle_1 : handle_2;

        for (uint32_t l = 0; l < 7; ++l) {
            klog(p_handle, l, "%d", l);
        }
    }

    for (uint32_t i = 0; i < 30; ++i) {
        const KlogLoggerHandle* p_handle = i % 2 ? handle_1 : handle_2;
        const uint32_t level = (i % 6) + 1; /* @todo kjk 2025/01/06 Magic number 6 */
        klog(p_handle, level, "Log %i", i);
    }

    klog_deinitialize();
}

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(void) {
    do_it(true);
    do_it(false);
    return 0;
}
