#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
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

/**
 * @brief Format the logger name (convert all whitespaces to underscores)
 * @details Convert all white spaces to underscores for the given logger name.
 *      The output is stored in the b_output parameter. If the provided logger
 *      name is shorter than max_length, then the output buffer will be appended
 *      with spaces after the logger name to make a string of max_length characters.
 *      All "whitespace" characters are converted to '_' characters. Whitespace
 *      characters are '\t', ' ', '\r', '\b', and '\0'.
 * @pre s_name must be non-NULL
 * @pre name_unformatted_length must be greater than 0
 * @pre b_output must be non-NULL and at least max_length bytes large
 * @pre max_length must be greater than 0
 * @param s_name                  The unformatted logger name
 * @param name_unformatted_length The length in characters of the unformatted logger
 *      name. This is so we don't need to rely upon null terminated logger name strings
 * @param b_output                The output buffer
 * @param max_length              The maximum length of the formatted logger name
 * @returns The formatted logger name is stored in b_output. The output will NOT be
 *      null terminated
 */
void klog_format_logger_name(
    const char*    s_name,
    const uint32_t name_unformatted_length,
    char*          b_output,
    const uint32_t max_length
);

/**
 * @brief Format the file name prefix
 * @details This uses klog_format_logger_name to perform the formatting
 * @pre s_name is null terminated
 * @param s_name   The unformatted file name
 * @param alloc_cb The allocation callback. This is used for allocating the output
 *      buffer which stores the formatted file prefix
 * @returns A null terminated string representing the formatted file name
 */
const char* klog_format_file_name_prefix(
    const char*    s_name,
    void* (* const alloc_cb)(
        size_t size
    )
);

KlogString klog_format_message_prefix(
    char*             s_prefix,
    const uint32_t*   p_thread_id,
    const KlogString* p_time,
    const KlogString* p_name,
    const KlogString* p_level,
    const KlogString* p_source_location
);

/* Returns the length of the full message (including the null terminating character) */
uint32_t klog_format_input_message(
    char*          b_output,
    const uint32_t size_output,
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
