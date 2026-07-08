/**
 * @file klog_version.c
 * @brief An example showcasing that we can use standard printf formatters. Also show
 *      how we can get the klog version
 */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo  format_info  = { 4, 100, 0, false, false };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(3, format_info, NULL, &console_info, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("klog", 4);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog_info(p_handle, "Klog version %d.%d.%d", KLOG_VERSION_MAJOR, KLOG_VERSION_MINOR, KLOG_VERSION_PATCH);

    klog_deinitialize();

    return 0;
}
