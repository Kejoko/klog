#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"
#include "./klog_constants.h"
#include "./klog_format.h"

pid_t klog_format_get_current_thread_id(void) {
    /* @todo kjk 2025/12/31 Call this once per thread so we can avoid switching into kernel mode repeatedly */
    return syscall(SYS_gettid);
}

const char* klog_format_get_level_string(const klog_format_context_t context, const enum klog_level_e requested_level) {
    /* @todo kjk 2025/12/31 Take level string buffer as parameter? Ensure it is correctly sized? */
    const uint32_t i_level_string = G_klog_level_string_length * requested_level;
    return &(context.p_level_strings[i_level_string]);
}

klog_format_split_t klog_format_split_strings(char *full_format) {
    printf("Splitting %s\n", full_format);

    const uint32_t input_strlen = strlen(full_format);

    uint32_t number_format_strings = 1;

    /* First pass to count how many format strings we will have, so we can allocate our result buffers accordingly */
    for (uint32_t i_char_newline_count = 0; i_char_newline_count < input_strlen; ++i_char_newline_count) {
        if (full_format[i_char_newline_count] == '\n') {
            number_format_strings += 1;
        }
    }

    /* Allocate the resulting buffers */
    char** format_strings = malloc(number_format_strings * sizeof(char*));
    uint32_t* format_string_lengths = malloc(number_format_strings * sizeof(uint32_t));
    uint32_t* va_arg_start_indices = malloc(number_format_strings * sizeof(uint32_t));
    uint32_t* va_arg_amount = malloc(number_format_strings * sizeof(uint32_t));

    /* Find all of the newlines so we know where the format string pointers should begin */
    /* Find all of the %'s to see where we have arguments */
    uint32_t i_curr_format_string = 0;
    uint32_t curr_format_string_start_index = 0;
    // Track the current number of % characters, so we can see if the immediately preceeding % count is odd or even
    uint32_t num_preceeding_percentages = 0;
    for (uint32_t i_char_populate = 0; i_char_populate < input_strlen; ++i_char_populate) {
        if (full_format[i_char_populate] == '\n') {
            /* Update the information for our current format string */
            const uint32_t curr_format_string_length = i_char_populate - curr_format_string_start_index;
            format_string_lengths[i_curr_format_string] = curr_format_string_length;
            
            /* Move on to the next format string */
            i_curr_format_string += 1;
            curr_format_string_start_index = i_char_populate + 1; /* +1 s we can skip over the current newline character */
            continue;
        }

        if (full_format[i_char_populate] == '%') {
            /* Ensure we are not printing a literal `%` character */
            /* This gets tricky, consider trying to print "%42", the format string would be "%%%d" - we can stack % characters infinitely */
            /* We need to backtrack to see if the number of `%` characters, including the current one, is even */
            if (i_char_populate > 0 && (num_preceeding_percentages % 2)) {

            }

            num_preceeding_percentages += 1;
            continue;
        }

        num_preceeding_percentages = 0;
    }

    klog_format_split_t result = {number_format_strings, format_strings, format_string_lengths, va_arg_start_indices, va_arg_amount};

    return result;
}

char* klog_format(const klog_format_context_t context, const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, va_list args) {
    const pid_t thread_id = klog_format_get_current_thread_id();
    const char* logger_name = &(context.p_logger_names[logger_handle.value * context.logger_name_max_length]);
    const char* level_string = klog_format_get_level_string(context, requested_level);
    /**
     * @brief String format is "XXXXX [logger_name] [level] <message>"
     * @details So we have
     *      - 5 characters for thread id
     *      - 1 for space
     *      - 2 for brackets around logger name
     *      - logger name length is determined by user
     *      - 1 for space
     *      - 2 for brackets around logger level
     *      - 8 for level
     *      - 1 for space
     *      - the message
     *      In total: (5+1+2+1+2+8+1 + logger name) + message
     *      In total: (20 + logger name) + message
     */

    /* We need to make a copy of the args (for the second vsnprintf call) before we consume them with the first vsnprintf call */
    va_list args_copy;
    va_copy(args_copy, args);

    /* Calculate the length of the input message */
    /* +1 for null termination */
    const int input_message_length = vsnprintf(0, 0, format, args) + 1;

    /* Actually create and format the input message, now that we know the length */
    char* input_message = malloc(input_message_length);
    vsnprintf(input_message, input_message_length, format, args_copy);
    va_end(args_copy);

    /* Create the resultant message, with the klog prefixes inserted at the front */
    const uint32_t prefix_length = 20 + context.logger_name_max_length;
    const uint32_t total_message_length = prefix_length + input_message_length;
    char* total_message = malloc(total_message_length);
    sprintf(total_message, "%5d [%.*s] [%.*s] %s", thread_id, context.logger_name_max_length, logger_name, G_klog_level_string_length, level_string, input_message);

    free(input_message);

    return total_message;
}
