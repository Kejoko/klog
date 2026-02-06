/**
 * @brief Ensure that we can log to a file AND NOT to the console correctly
 */

#include "klog/klog.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_HANDLES 4
#define NUM_LOGGING_STATEMENTS 100

int main(void) {
    KlogFormatInfo format_info = {8, 10, 0, true, true};
    KlogFileInfo file_info = {KLOG_LEVEL_INFO, "FILE__PREFIX____OR_WHATEVER_THIS_PREFIX_IS_LONG"};
    klog_initialize(4, format_info, NULL, NULL, &file_info);

    const KlogLoggerHandle* handle_1 = klog_logger_create("MyLogger");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog(handle_1, KLOG_LEVEL_TRACE, "This should not appear due to the logger's max verbosity");
    klog(handle_1, KLOG_LEVEL_DEBUG, "This should not appear due to the file's max verbosity");
    klog(handle_1, KLOG_LEVEL_INFO, "Hello!!!!!");
    klog(handle_1, KLOG_LEVEL_WARN, "Hello    -Everything after the hyphen should not be printed due to the max message length");
    klog(handle_1, KLOG_LEVEL_ERROR, "Big Error");
    klog(handle_1, KLOG_LEVEL_FATAL, "Huge Error");

    const KlogLoggerHandle* handle_2 = klog_logger_create("Boo With Some Spaces In Here");
    klog_logger_set_level(handle_2, KLOG_LEVEL_ERROR);
    klog(handle_2, KLOG_LEVEL_ERROR, "Truncate__Logging at the max verbosity level for the second logger");

    const KlogLoggerHandle* handle_3 = klog_logger_create("ABC");
    const KlogLoggerHandle* handle_4 = klog_logger_create("  ..  \\\\AAAAAAA");

    const KlogLoggerHandle* a_handles[NUM_HANDLES] = {handle_1, handle_2, handle_3, handle_4};
    for (uint32_t i = 0; i < NUM_HANDLES; ++i) {
        klog_logger_set_level(a_handles[i], KLOG_LEVEL_INFO);
    }
    for (uint32_t i = 0; i < NUM_LOGGING_STATEMENTS; ++i) {
        klog_info(a_handles[i % NUM_HANDLES], "%d : %d", i, i*i);
    }

    klog_deinitialize();
}
