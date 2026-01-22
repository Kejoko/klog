#include "klog/klog.h"

#include <stdio.h>

/* This is testing klog with max loggers set to 2, and name length set to 6 */
int main(void) {
    KlogFormatInfo format_info = {6, 100, false, false};
    klog_initialize(2, format_info, NULL, NULL, NULL);

    const KlogLoggerHandle* handle_1 = klog_logger_create("MyLogger");
    klog(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog(handle_1, KLOG_LEVEL_TRACE, "This should also not appear");
    klog(handle_1, KLOG_LEVEL_INFO, "This should appear with the format %d with %f stuff %s - first log statement", 42, 42.42f, "fourty two");

    const KlogLoggerHandle* handle_2 = klog_logger_create("B");
    klog_logger_set_level(handle_2, 6);
    klog(handle_2, KLOG_LEVEL_TRACE, "What's up - trace level - second log statement");

    const char* name_3 = "ABC";
    const KlogLoggerHandle* handle_3 = klog_logger_create(name_3);
    klog(handle_3, KLOG_LEVEL_TRACE, "We should fail before we ever get here");

    klog_deinitialize();
}
