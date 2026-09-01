/**
 * @brief Minimal example showcasing easy setup for async logging
 */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo  format_info  = { .logger_name_max_length = 6, .message_max_length = 50 };
    KlogAsyncInfo   async_info   = { .message_queue_number_elements = 10, .number_backing_threads = 1 };
    KlogConsoleInfo console_info = { .max_level = KLOG_LEVEL_DEBUG };
    klog_initialize(4, format_info, &async_info, &console_info, NULL, NULL);

    const KlogLoggerHandle* handle = klog_logger_create("My Logger", 8);
    klog_logger_level_set(handle, KLOG_LEVEL_INFO);

    klog(handle, KLOG_LEVEL_INFO, "Hello\nworld!");

    klog_deinitialize();

    return 0;
}
