/**
 * @file async.c
 * @brief An example executable showcasing how to set up asynchronous logging.
 * @note The thread id reported by klog is the thread id of the calling thread,
 *      so all of the thread ids reported here will be the id of this main
 *      functions thread. The thread ids reported ARE NOT the thread id of the
 *      backing klog threads.
 */

#include "klog/klog.h"

int main(
    void
) {
    /* Set the thread id logging to true to display logging thread's ids */
    KlogFormatInfo  format_info = { 6, 100, 0, true, false };
    KlogAsyncInfo   async_info  = {
        20, /* Maximum of 30 elements in the message queue before any blocking occurs */
        3,  /* The number of backing threads that consume the log statements from the message queue */
    };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(3, format_info, &async_info, &console_info, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("logga", 5);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);

    for (uint32_t i = 0; i < 60; ++i) {
        klog(p_handle, KLOG_LEVEL_TRACE, "%d", i);
    }

    klog_deinitialize();

    return 0;
}
