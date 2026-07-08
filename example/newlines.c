/**
 * @file newlines.c
 * @brief An example showing klog's behavior when newlines are introduced into the log statement.
 * @details The maximum character limit is for the maximum number of characters in the incoming
 *      string, not the maximum number of characters on a single printed line. You can produce
 *      multiple printed lines with one input string, but it is the input string which is
 *      clamped by the max length (prior to being split into multiple lines).
 */

#include "klog/klog.h"

int main(
    void
) {
    KlogFormatInfo  format_info  = { 10, 18, 0, false, false };
    KlogConsoleInfo console_info = { KLOG_LEVEL_TRACE, true };
    klog_initialize(3, format_info, NULL, &console_info, NULL, NULL);

    const KlogLoggerHandle* p_handle = klog_logger_create("my_logger", 9);
    klog_logger_level_set(p_handle, KLOG_LEVEL_TRACE);

    /* This will produce multiple log statements due to the newline */
    klog_info(p_handle, "Hello\nworld!");

    /**
     * This showcases that newlines count towards the character count.
     * With a max limit of 18 chars we will print "ABCDEFG\n1234567\nab".
     * The characters following the 'b' will not be printed because they
     * exceed the 18 character limit we have set.
     */
    /* 00 +              1234567 89                                    */
    /* 10 +                      | 012345 6789                         */
    /* 20 +                               |   0123456789               */
    klog_info(p_handle, "ABCDEFG\n1234567\nab-----------");

    klog_deinitialize();

    return 0;
}
