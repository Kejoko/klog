/**
 * @file basic_file.c
 * @brief An example executable showcasing how to set up a basic file logger.
 * @note The final log file name created by this executable will have a date
 *      and timestamp appended to it, so it will be something like:
 *      my_super_cool_file_202607188_191359_0793.log
 * @details The filename is formatted like <prefix>_YYYYMMDD_HHMMSS_SSSS.log
 */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo format_info = { 6, 100, 0, false, false };
    KlogFileInfo   file_info   = {
        KLOG_LEVEL_INFO,     /* The maximum verbosity of logs accepted by the file */
        "my_super_cool_file" /* The base filename of the logfile - the date and timestamp are appended to this */
    };
    klog_initialize(3, format_info, NULL, NULL, &file_info, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("spigus", 6);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);
    klog(p_handle, KLOG_LEVEL_INFO,  "Logging at info level :)");
    klog(p_handle, KLOG_LEVEL_DEBUG, "This will not log because this is less than info level!");

    klog_deinitialize();

    return 0;
}
