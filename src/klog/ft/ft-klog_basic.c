/**
 * @brief Just a basic klog test. Testing nothing in particular, just general functionality
 */

#include "klog/klog.h"

#include <stdio.h>
#include <stdlib.h>

#include "../klog_debug_util.h"

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(void) {
    KlogFormatInfo format_info = {6, 100, 10, true, true};
    KlogConsoleInfo console_info = {KLOG_LEVEL_INFO, true};
    KlogFileInfo file_info = {KLOG_LEVEL_TRACE, "BASIC\tPREFIX"};
    kdprintf("INITIALIZING KLOG\n");
    klog_initialize(4, format_info, NULL, &console_info, &file_info);

    kdprintf("CREATING HANDLE\n");
    const KlogLoggerHandle* handle_1 = klog_logger_create("My Logger");
    kdprintf("LOGGING BEFORE SETTING LEVEL\n");
    klog(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    kdprintf("SETTING LEVEL TO DEBUG\n");
    klog_logger_level_set(handle_1, KLOG_LEVEL_DEBUG);
    kdprintf("LOGGING AT TRACE LEVEL\n");
    klog(handle_1, KLOG_LEVEL_TRACE, "This should also not appear");
    kdprintf("LOGGING AT INFO LEVEL\n");
    klog(handle_1, KLOG_LEVEL_INFO, "This should appear with the format %d with %f stuff %s - first log statement", 42, 42.42f, "fourty two");

#ifndef KLOG_DEBUG
    const KlogLoggerHandle* handle_2 = klog_logger_create("B");
    klog_logger_level_set(handle_2, 6);
    klog(handle_2, KLOG_LEVEL_TRACE, "What's up - trace level - second log statement");

    const char* name_3 = "ABC";
    const KlogLoggerHandle* handle_3 = klog_logger_create(name_3);
    const KlogLoggerHandle* handle_3_custom = handle_3;
    klog_logger_level_set(handle_3_custom, KLOG_LEVEL_INFO);
    klog(handle_3_custom, 6, "Logger ABC should not log trace");
    klog(handle_3, 4, "Logger ABC should log this - third log statement");
    klog_logger_level_set(handle_3_custom, 5);
    klog(handle_3, 6, "Logger ABC should not log this");
    klog(handle_3, 5, "Logger ABC should log this too - fourth log statement");
    klog(handle_3, KLOG_LEVEL_WARN, "This should be an warning.");
    klog(handle_3, KLOG_LEVEL_ERROR, "This should be an error!!");
    klog(handle_3, KLOG_LEVEL_FATAL, "THIS SHOULD BE FATAL!!!!!!!!!");

    const KlogLoggerHandle* handle_4_custom = klog_logger_create("123456");
    klog(handle_4_custom, 5, "Logging with an off logger shouldn't do anything");
    klog_logger_level_set(handle_4_custom, KLOG_LEVEL_INFO);
    klog(handle_4_custom, KLOG_LEVEL_INFO, "Multiple formats (one - %d)\ntwo\n\n%s%d\n5", 1, "four", 4);
#endif

    kdprintf("DEINITIALIZING KLOG\n");
    klog_deinitialize();

    return 0;
}
