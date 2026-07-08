/**
 * @file console_and_file.c
 * @brief An example executable showcasing how to set up a console logger and
 *      a file logger, and log to both.
 * @note The file and console loggers can filter to different log levels than
 *      eachother, so one can allow more verbose logs to come through than the
 *      other.
 * @note Console can have colored log statements (log levels) but the file cannot.
 */

#include "klog/klog.h"

int main(
    void
) {
    /* Allow all logs statements to console, while file only accepts info and higher */
    KlogFormatInfo  format_info  = { 6, 100, 0, false, true };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    KlogFileInfo    file_info    = { KLOG_LEVEL_INFO, "console_and_file" };
    klog_initialize(3, format_info, NULL, &console_info, &file_info, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("hehehe", 6);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog(p_handle, KLOG_LEVEL_INFO,  "Logging at info level - should go to both console and file");
    klog(p_handle, KLOG_LEVEL_DEBUG, "This will only go to the console");

    klog_deinitialize();

    return 0;
}
