/**
 * @file basic_console.c
 * @brief An example executable showcasing how to set up a basic console logger.
 *      The log statements in this executable will get logged to the console, but
 *      will not get logged to any files. This is a basic example to show how you
 *      can create and use loggers with different levels.
 */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo  format_info = {
        6,     /* Maximum length of a logger's name */
        100,   /* Maximum length of a message */
        0,     /* Maximum length of source location filename - 0 to turn this off */
        false, /* Whether or not to use the thread id */
        false  /* Whether or not to log with a timestamp */
    };
    KlogConsoleInfo console_info = {
        KLOG_LEVEL_DEBUG, /* The maximum verbosity of logs accepted by the console */
        true              /* Whether or not to log the levels in color - obviously we want colors! */
    };
    klog_initialize(
        3,             /* The maximum number of loggers we can create */
        format_info,   /* The formatting info - this is always required */
        NULL,          /* Async information - this example is logging synchronously so we're not providing this */
        &console_info, /* The information about console logging */
        NULL,          /* The information about file logging - this example does not care about this */
        NULL           /* Allocation callbacks - this example is going to use malloc and free so we won't provide this */
    );

    /* Create a logger named "spigus" and set its level to info. By default loggers are off */
    const KlogLoggerHandle* p_handle1 = klog_logger_create("spigus", 6);
    klog_logger_level_set(p_handle1, KLOG_LEVEL_INFO);
    /* Log some stuff with our fresh logger */
    klog(p_handle1, KLOG_LEVEL_INFO,  "Logging at info level :)");
    klog(p_handle1, KLOG_LEVEL_WARN,  "Creating a warning :O");
    klog(p_handle1, KLOG_LEVEL_ERROR, "An error!");
    klog(p_handle1, KLOG_LEVEL_FATAL, "We gonna die :'(");
    klog(p_handle1, KLOG_LEVEL_DEBUG, "This will not log because this logger only accepts logs greater than or equal to info level!");

    /* Create a logger named "splointus" (which will truncate to "sploin" and set its level to the max verbosity */
    const KlogLoggerHandle* p_handle2 = klog_logger_create("splointus", 8);
    klog_logger_level_set(p_handle2, KLOG_LEVEL_TRACE);
    /* Log some stuff with our even fresh logger */
    klog_debug(p_handle2, "Splointus is debugging using the cool level specific macros B)");
    klog_info(p_handle2, "We actually have a logging macro for each of the levels!");
    klog_warn(p_handle2, "This is the warning - scary!");
    klog_error(p_handle2, "This is what an error looks like!");
    klog_fatal(p_handle2, "Another fatal error nooooooooooo");
    klog_trace(p_handle2, "This will not get logged because our console logger only accepts debug logs at max");

    /* Always remember to deinitialize to prevent memory leaks! */
    klog_deinitialize();

    return 0;
}
