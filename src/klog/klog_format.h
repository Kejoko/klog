#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

#include <stdarg.h>
#include <stdint.h>

#ifdef  __linux__
#include <unistd.h>
#endif

#include "klog/klog.h"

typedef struct {
    uint32_t length;
    const char* s;
} KlogString;

typedef struct {
    /* The number of resulting format strings */
    const uint32_t  number_strings;

    /* An array of format strings, each index of this points to a format string */
    const char** const strings;

    /* An array of lengths, so we know how long each of the format strings are */
    const uint32_t* const string_lengths;
} KlogFormatSplitInfo;

pid_t klog_format_get_current_thread_id(void);

const char*         klog_format_input_message(const char* format, va_list args);
KlogFormatSplitInfo klog_format_split_strings(const char* formatted_input);
KlogString          klog_format_time(void);
KlogString          klog_format_source_location(const uint32_t filename_size_max, const char* s_filepath, const uint32_t line);

#endif /* KLOG_FORMAT_INCLUDED */
