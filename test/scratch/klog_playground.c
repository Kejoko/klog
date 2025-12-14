#include "klog/klog.h"

/* This is testing klog with max loggers set to 4, and name length set to 6 */
int main(void) {
    klog_initialize();

    const klog_logger_handle_t handle_1 = klog_logger_create("MyLogger");
    klog_log(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog_log(handle_1, KLOG_LEVEL_INFO, "This should also not appear");
    klog_log(handle_1, KLOG_LEVEL_INFO, "This should appear with the format %d with %f stuff %s", 42, 42.42f, "fourty two");

    /* When compiled in, this should cause a crash due to using a logger handle that doesn't exist */
#if 0
    klog_log(1, KLOG_LEVEL_INFO, "This should crash the program");
#endif

    const klog_logger_handle_t handle_2 = klog_logger_create("B");
    klog_logger_set_level(handle_2, 0);
    klog_log(handle_2, 0, "What's up - trace level");

    const char* name_3 = "ABC";
    const klog_logger_handle_t handle_3 = klog_logger_create(name_3);
    klog_logger_set_level(2, 4);
    klog_log(2, 0, "Logger ABC should not log trace");
    klog_log(handle_3, 4, "Logger ABC should log this");
    klog_logger_set_level(2, 5);
    klog_log(handle_3, 4, "Logger ABC should not log this");
    klog_log(handle_3, 5, "Logger ABC should log this");

    /* When compiled in, this should cause a crash due to setting the level for a logger handle that doesn't exist */
#if 0
    klog_logger_set_level(3, 5);
#endif

    klog_logger_create("123456");
    klog_log(3, 5, "Logging with an off logger shouldn't do anything");

    /* When compiled in, this should cause a crash due to creating more loggers than we are allowed */
#if 0
    klog_logger_create("error");
#endif

    klog_deinitialize();
}
