#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./klog_format.h"
#include "./klog_platform.h"

/**
 * @brief Output the message to both the console and file if they are enabled
 *      and if the requested level passes the gate.
 * @details Print each line (denoted by newlines) to the given output channels.
 *      Find the next newline, and create a KlogString representing the unlogged
 *      portion up until that newline. Each logged line uses the same message
 *      prefix.
 * @note If level_requested is more verbose than level_max_console or level_max_file
 *      then the message will not be logged to that respective sink.
 * @pre s_message_formatted must be allocated for at least actual_message_length
 *      bytes.
 * @pre If the request level passes the file's level gate, p_file must be non-NULL.
 * @param actual_message_length The actual length of the input message.
 * @param s_message_formatted   The buffer containing the actual formatted message.
 * @param packed_prefix_console The message prefix to use when logging to the console.
 * @param packed_prefix_file    The message prefix to use when logging to the file.
 * @param level_requested       The requested log level.
 * @param level_max_console     The maximum verbosity allowed for console logging.
 * @param level_max_file        The maximum verbosity allowed for file logging.
 * @param p_file                The file pointer at which to log.
 * @param p_mutex_console       The mutex for console output. This is so multiple threads
 *      don't try to write to the console at the same time, clobbering messages.
 * @param p_mutex_file          The mutex for file output. This is so multiple threads
 *      don't try to write to the console at the same time, clobbering messages.
 */
void klog_output(
    uint32_t               actual_message_length,
    const char*            s_message_formatted,
    KlogString             packed_prefix_console,
    KlogString             packed_prefix_file,
    uint8_t                level_requested,
    uint8_t                level_max_console,
    uint8_t                level_max_file,
    FILE*                  p_file,
    klog_platform_mutex_t* p_mutex_console,
    klog_platform_mutex_t* p_mutex_file
);

/**
 * @brief Output the prefix and the message to the console.
 * @param p_prefix  The KlogString containing the prefix and its length.
 * @param p_message The KlogString containing the message and its length.
 */
void klog_output_console(
    const KlogString* p_prefix,
    const KlogString* p_message
);

/**
 * @brief Output the prefix and the message to the file pointer.
 * @pre p_file must be non-NULL.
 * @param p_file    The file pointer to which we are going to output.
 * @param p_prefix  The KlogString containing the prefix and its length.
 * @param p_message The KlogString containing the message and its length.
 */
void klog_output_file(
    FILE* const       p_file,
    const KlogString* p_prefix,
    const KlogString* p_message
);

#endif /* KLOG_OUTPUT_INCLUDED */
