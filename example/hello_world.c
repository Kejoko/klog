/**
 * @file hello_world.c
 * @brief A minimal example to print hello world with klog
 */

#include "klog/klog.h"

int main(
    void
) {
    /* We are using relatively short lines (10 chars) to easily display newline behavior */
    KlogFormatInfo  format_info  = { 10, 100, 0, false, false };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(3, format_info, NULL, &console_info, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("my_logger", 9);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog_info(p_handle, "Hello world!");
    klog_info(p_handle, "We can use printf formatting, look! %s %#x %f %c%c", "42", 66, 42.42f, 52, '2');

    klog_deinitialize();

    return 0;
}
