#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t          length;
    const char* const s;
} KlogString;

uint32_t klog_format_prefix_length_get(
    const bool     use_thread_id,
    const bool     use_timestamp,
    const uint32_t logger_name_max_length,
    const bool     use_color,
    const uint32_t source_location_filename_max_length
);

const char* klog_format_logger_name(
    const char* s_name
);

const char* klog_format_file_name_prefix(
    const char* s_name
);

KlogString klog_format_message_prefix(
    char*             s_prefix,
    const uint32_t*   p_thread_id,
    const KlogString* p_time,
    const KlogString* p_name,
    const KlogString* p_level,
    const KlogString* p_source_location
);

/* Returns the length of the full message (not including null terminating character) */
uint32_t klog_format_input_message(
    char*          b_output,
    const uint32_t output_size,
    const char*    s_format,
    va_list        args
);

KlogString klog_format_time(
    char* s_time
);

KlogString klog_format_source_location(
    char*          s_time,
    const uint32_t filename_size_max,
    const char*    s_filepath,
    const uint32_t line_number
);

#endif /* KLOG_FORMAT_INCLUDED */
