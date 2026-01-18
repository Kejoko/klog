#include "klog/klog.h"

#include <stdio.h>

int main(void) {
    /* Fail because klog is uninitialized */
    const KlogLoggerHandle handle_1 = klog_logger_create("MyLogger");
    klog(handle_1, KLOG_LEVEL_INFO, "This should not appear");
    klog_logger_set_level(handle_1, KLOG_LEVEL_DEBUG);
    klog(handle_1, KLOG_LEVEL_TRACE, "This should also not appear");

    klog_deinitialize();
    printf("Done! - via stdout\n");
}
