/**
 * @file extra_formatting.c
 * @brief An example showcasing the extra formatting options that we can use (source filename,
 *      timestamp, thread id).
 */

#include <unistd.h> /* for sleep */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo  format_info = {
        7,    /* Maximum length of a logger's name */
        100,  /* Maximum length of a message */
        12,   /* Maximum length of source location filename - 0 to turn this off */
        true, /* Whether or not to use the thread id */
        true  /* Whether or not to log with a timestamp */
    };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(3, format_info, NULL, &console_info, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("My Name", 7);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog_info(p_handle, "Notice how \"My Name\" was converted to \"My_Name\"");
    klog_info(p_handle, "and how there is much more information in the prefix :)");
    klog_info(p_handle, "The first value is the thread id");
    klog_info(p_handle, "The second value is the timestamp of the log statement");
    sleep(1);
    klog_info(p_handle, "  notice the difference in timestamps between this log statement and the last!");
    klog_info(p_handle, "The value after the log level is the source location filename and line of the current log statement");
    klog_info(p_handle, "  notice the increase in line numbers for each log statement");
    klog_info(p_handle, "  and notice that the filename got truncated because we're only allowing 12 characters");

    klog_deinitialize();

    return 0;
}
