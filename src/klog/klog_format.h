#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "klog/klog.h"

/**
 * @brief C style string utility.
 * @note s does not need to be null terminated.
 * @note This is so we can pass around pointers and a corresponding length nicely
 *      without having to introduce a second parameter in many places.
 */
typedef struct {
    uint32_t          length;
    const char* const s;
} KlogString;

/**
 * @brief This removes the whitespace from the provided prefix, and returns a newly
 *      allocated null terminated string containing the sanitized prefix and a timestamp.
 * @note This performs two allocations and one free using the provided allocation and
 *      free callbacks. The resulting filename is heap allocated, so ensure you free it
 *      later.
 * @pre p_klog_file_info is non-NULL.
 * @pre p_klog_file_info contains a non-NULL filename prefix.
 * @pre p_klog_file_info contains a filename prefix with length greater than 0.
 * @param p_klog_file_info The file information struct.
 * @param alloc_cb         The memory allocation callback.
 * @param free_cb          The memory free callback.
 * @returns A heap allocated, null terminated string.
 */
char* klog_format_filename(
    const KlogFileInfo* const p_klog_file_info,
    void* (* const            alloc_cb)(
        size_t size
    ),
    void (*                   free_cb)(
        void*
    )
);

/**
 * @brief Determine the length of the message prefix.
 * @details This determines how long the prefix will be for every message logged. This
 *      is important to calculate upfront so we can aptly allocate the buffers needed
 *      to store the message prefixes.
 * @note This can be different for console vs file, based on colors, etc.
 * @param use_thread_id          Whether or not we will be displaying the thread id of the
 *      calling thread.
 * @param use_timestamp          Whether or not we will be displaying the timestamp of the
 *      log statement.
 * @param logger_name_length_max How long the logger's name can be.
 * @param use_color              Whether or not we are logging with colored levels.
 * @param source_length_max      The length of the source file string.
 * @returns An unsinged integer representing the number of characters that the prefix
 *      will be, including the null terminating character.
 */
uint32_t klog_format_prefix_length_get(
    const bool     use_thread_id,
    const bool     use_timestamp,
    const uint32_t logger_name_length_max,
    const bool     use_color,
    const uint32_t source_length_max
);

/**
 * @brief Format the logger name (convert all whitespaces to underscores).
 * @details Convert all white spaces to underscores for the given logger name.
 *      The output is stored in the b_output parameter. If the provided logger
 *      name is shorter than max_length, then the output buffer will be appended
 *      with spaces after the logger name to make a string of max_length characters.
 *      All "whitespace" characters are converted to '_' characters. Whitespace
 *      characters are '\t', ' ', '\r', '\b', and '\0'.
 * @pre s_name must be non-NULL.
 * @pre name_unformatted_length must be greater than 0.
 * @pre b_output must be non-NULL and at least max_length bytes large.
 * @pre max_length must be greater than 0.
 * @param s_name                  The unformatted logger name.
 * @param name_unformatted_length The length in characters of the unformatted logger
 *      name. This is so we don't need to rely upon null terminated logger name strings.
 * @param b_output                The output buffer.
 * @param max_length              The maximum length of the formatted logger name.
 * @returns The formatted logger name is stored in b_output. The output will NOT be
 *      null terminated.
 */
void klog_format_logger_name(
    const char*    s_name,
    const uint32_t name_unformatted_length,
    char*          b_output,
    const uint32_t max_length
);

/**
 * @brief Format the file name prefix.
 * @details This uses klog_format_logger_name() to perform the formatting.
 * @pre s_name is null terminated.
 * @param s_name   The unformatted file name.
 * @param alloc_cb The allocation callback. This is used for allocating the output
 *      buffer which stores the formatted file prefix.
 * @returns A null terminated string representing the formatted file name.
 */
const char* klog_format_file_name_prefix(
    const char*    s_name,
    void* (* const alloc_cb)(
        size_t size
    )
);

/**
 * @brief Format the message prefix.
 * @details Given all fields, the total length is given by
 *      8[thread id and space] +
 *      (p_time.length+1)[timestamp and space] +
 *      (p_name.length+3)[logger name, brackets, space] +
 *      (p_level+3)[level name, brackets, space] +
 *      (p_source_location.length+3)[source location, brackets, space] +
 *      1[null terminator]
 * @pre s_prefix is non-NULL.
 * @pre s_prefix is allocated with enough space to hold the entire prefix.
 * @param s_prefix          The output buffer in which to store the formatted prefix.
 * @param p_thread_id       The thread id.
 * @param p_time            The string representing the timestamp.
 * @param p_name            The string representing the logger name.
 * @param p_level           The string representing the level.
 * @param p_source_location The string representing the source location.
 * @returns A KlogString containing a null terminated char* and the length. The length
 *      includes the null terminatig character.
 */
KlogString klog_format_message_prefix(
    char*             s_prefix,
    const uint32_t*   p_thread_id,
    const KlogString* p_time,
    const KlogString* p_name,
    const KlogString* p_level,
    const KlogString* p_source_location
);

/**
 * @brief Format the message into the given output buffer, and calculate the formatted
 *      message's length.
 * @details This is effectively just a wrapper around vsnprintf.
 * @pre size_output is greater than 0.
 * @param b_output    The output buffer.
 * @param size_output The size of the output buffer.
 * @param s_format    The format string.
 * @param args        The arguments to format.
 * @returns The length of the formatted message, including the null terminating character.
 */
uint32_t klog_format_input_message(
    char*          b_output,
    const uint32_t size_output,
    const char*    s_format,
    va_list        args
);

/**
 * @brief Format the current time.
 * @details Get the urrent time using klog_platform_get_current_timepoint(), and
 *      create a string in the format "%.3d:%.2d:%.2d:%.2d:%.6d". This results in
 *      a string of length G_klog_time_string_length.
 * @pre s_time is non-NULL.
 * @pre s_time is allocated with at least G_klog_time_string_length bytes.
 * @param s_time The output buffer in which to store the formatted time string.
 * @returns A KlogString containing the formatted time in s_time, and the length
 *      of the formatted time string. KlogString{s_time, G_klog_time_string_length}.
 */
KlogString klog_format_time(
    char* s_time
);

/**
 * @brief Format the filename and line number into the desired format.
 * @note This clamps line numbers at 9999.
 * @pre filename_size_max is greater than 0.
 * @pre s_source_location is non-NULL.
 * @pre s_source_location is allocated with enough space to store the formatted source location.
 * @param s_source_location The output buffer where the formatted source location will go.
 * @param filename_size_max The maximum length of the filename to use.
 * @param s_filepath        The filename to use.
 * @param line_number       The line number to use.
 * @returns A KlogString with the formatted source location, and the length of the formatted
 *      string. The char* field of the KlogString will be set to s_source_location.
 */
KlogString klog_format_source_location(
    char*          s_source_location,
    const uint32_t filename_size_max,
    const char*    s_filepath,
    const uint32_t line_number
);

#endif /* KLOG_FORMAT_INCLUDED */
