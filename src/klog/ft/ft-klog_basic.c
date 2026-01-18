#include "klog/klog.h"

#include <stdio.h>

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(void) {
    klog_initialize(4, 6, 1, 10, 0, NULL, NULL);

    const KlogLoggerHandle* handle_1 = klog_logger_create("MyLogger");
    klog(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog(handle_1, KLOG_LEVEL_TRACE, "This should also not appear");
    klog(handle_1, KLOG_LEVEL_INFO, "This should appear with the format %d with %f stuff %s - first log statement", 42, 42.42f, "fourty two");

#if 0 /* When compiled in, this should cause a crash due to using a logger handle that doesn't exist */
    const KlogLoggerHandle handle_1_custom = handle_1;
    klog(handle_1_custom, KLOG_LEVEL_INFO, "This should crash the program");
#endif

    const KlogLoggerHandle* handle_2 = klog_logger_create("B");
    klog_logger_set_level(handle_2, 6);
    klog(handle_2, KLOG_LEVEL_TRACE, "What's up - trace level - second log statement");

    const char* name_3 = "ABC";
    const KlogLoggerHandle* handle_3 = klog_logger_create(name_3);
    const KlogLoggerHandle* handle_3_custom = handle_3;
    klog_logger_set_level(handle_3_custom, 4);
    klog(handle_3_custom, 6, "Logger ABC should not log trace");
    klog(handle_3, 4, "Logger ABC should log this - third log statement");
    klog_logger_set_level(handle_3_custom, 5);
    klog(handle_3, 6, "Logger ABC should not log this");
    klog(handle_3, 5, "Logger ABC should log this too - fourth log statement");

    const KlogLoggerHandle* handle_4_custom = klog_logger_create("123456");
#if 0 /* When compiled in, this should cause a crash due to setting the level for a logger handle that doesn't exist */
    klog_logger_set_level(handle_4_custom, 5);
#endif
    klog(handle_4_custom, 5, "Logging with an off logger shouldn't do anything");
    klog_logger_set_level(handle_4_custom, KLOG_LEVEL_INFO);
    klog(handle_4_custom, KLOG_LEVEL_INFO, "Multiple formats (one - %d)\ntwo\n\n%s%d\n5", 1, "four", 4);

#if 0 /* When compiled in, this should cause a crash due to creating more loggers than we are allowed */
    klog_logger_create("error");
#endif

    klog_deinitialize();
    printf("Done! - via stdout\n");
}
