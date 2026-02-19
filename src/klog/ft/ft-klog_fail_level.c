/**
 * @brief Ensure that the program exits if we try to log with a level that is greater than trace
 */

#include <stdlib.h>

#include "klog/klog.h"

int main(void) {
    klog_initialize(1, (KlogFormatInfo){10, 5, 0, false, false}, NULL, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("ABCDE");

    const uint32_t desired_level = KLOG_LEVEL_TRACE + 1;
    klog(p_handle, desired_level, "This should cause a program exit");

    klog_deinitialize();

    return 0;
}
