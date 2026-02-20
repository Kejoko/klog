#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t length;
    const char* const s;
} KlogString;

typedef struct {
    /* The number of resulting format strings */
    const uint32_t  number_strings;

    /* An array of format strings, each index of this points to a format string */
    const char** const ls_strings;

    /* An array of lengths, so we know how long each of the format strings are */
    const uint32_t* const a_string_lengths;
} KlogFormatSplitInfo;

uint32_t klog_format_prefix_length_get(const bool use_thread_id, const bool use_timestamp, const uint32_t logger_name_max_length, const bool use_color, const uint32_t source_location_filename_max_length);

const char*         klog_format_logger_name(const char* s_name);
const char*         klog_format_file_name_prefix(const char* s_name);
KlogString          klog_format_message_prefix(const uint32_t* p_thread_id, const KlogString* p_time, const KlogString* p_name, const KlogString* p_level, const KlogString* p_source_location);
const char*         klog_format_input_message(const char* s_format, va_list args);
KlogFormatSplitInfo klog_format_split_strings(const char* s_message);
KlogString          klog_format_time(void);
KlogString          klog_format_source_location(const uint32_t filename_size_max, const char* s_filepath, const uint32_t line_number);

#endif /* KLOG_FORMAT_INCLUDED */
