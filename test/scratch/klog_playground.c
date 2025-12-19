#include "klog/klog.h"

#include <stdio.h>

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(void) {
    klog_initialize(4, 6);

    const klog_logger_handle_t handle_1 = klog_logger_create("MyLogger");
    klog(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog(handle_1, KLOG_LEVEL_TRACE, "This should also not appear");
    klog(handle_1, KLOG_LEVEL_INFO, "This should appear with the format %d with %f stuff %s - first log statement", 42, 42.42f, "fourty two");

    /* When compiled in, this should cause a crash due to using a logger handle that doesn't exist */
#if 0
    klog(1, KLOG_LEVEL_INFO, "This should crash the program");
#endif

    const klog_logger_handle_t handle_2 = klog_logger_create("B");
    klog_logger_set_level(handle_2, 6);
    klog(handle_2, KLOG_LEVEL_TRACE, "What's up - trace level - second log statement");

    const char* name_3 = "ABC";
    const klog_logger_handle_t handle_3 = klog_logger_create(name_3);
    klog_logger_set_level(2, 4);
    klog(2, 6, "Logger ABC should not log trace");
    klog(handle_3, 4, "Logger ABC should log this - third log statement");
    klog_logger_set_level(2, 5);
    klog(handle_3, 6, "Logger ABC should not log this");
    klog(handle_3, 5, "Logger ABC should log this too - fourth log statement");

    /* When compiled in, this should cause a crash due to setting the level for a logger handle that doesn't exist */
#if 0
    klog_logger_set_level(3, 5);
#endif

    klog_logger_create("123456");
    klog(3, 5, "Logging with an off logger shouldn't do anything");

    /* When compiled in, this should cause a crash due to creating more loggers than we are allowed */
#if 0
    klog_logger_create("error");
#endif

    klog_deinitialize();
    printf("Done! - via stdout\n");
}
