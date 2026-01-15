#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"
#include "./klog_constants.h"
#include "./klog_debug_util.h"
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

const char* klog_format_input_message(const char* format, va_list p_args) {
    /* We need to make a copy of the args (for the second vsnprintf call) before we consume them with the first vsnprintf call */
    va_list p_args_copy;
    va_copy(p_args_copy, p_args);

    /* Calculate the length of the input message */
    /* +1 for null termination */
    /* @todo kjk 2026/01/14 Use _vscprintf */
    const int input_message_length = vsnprintf(0, 0, format, p_args) + 1;

    /* Actually create the input message, now that we know the length */
    char* const input_message = malloc(input_message_length);
    
    /*  Format the input message with the unused copy of the args */
    vsnprintf(input_message, input_message_length, format, p_args_copy);
    va_end(p_args_copy);

    return input_message;
}

klog_format_split_t klog_format_split_strings(const char* full_format) {
    const uint32_t input_strlen = strlen(full_format);

    uint32_t number_format_strings = 1;

    /* First pass to count how many format strings we will have, so we can allocate our result buffers accordingly */
    for (uint32_t i_char_newline_count = 0; i_char_newline_count < input_strlen; ++i_char_newline_count) {
        if (full_format[i_char_newline_count] == '\n') {
            number_format_strings += 1;
        }
    }

    /* Allocate the resulting buffers */
    const char** const format_strings = malloc(number_format_strings * sizeof(char*));
    uint32_t* const format_string_lengths = malloc(number_format_strings * sizeof(uint32_t));

    /* Initialize the first format string values in case we never reach a newline */
    format_strings[0] = full_format;
    format_string_lengths[0] = input_strlen;

    /* Find all of the newlines so we know where the format string pointers should begin */
    uint32_t curr_format_string_start_index = 0;
    uint32_t i_curr_format_string = 0;
    for (uint32_t i_char_base = 0; i_char_base < input_strlen; ++i_char_base) {
        if (full_format[i_char_base] != '\n') {
            continue;
        }

        /* Update the information for our current format string */
        const uint32_t curr_format_string_length = i_char_base - curr_format_string_start_index;
        format_string_lengths[i_curr_format_string] = curr_format_string_length;
        if (i_curr_format_string < (number_format_strings - 1)) {
            format_strings[i_curr_format_string+1] = full_format + i_char_base + 1;
        }

        /* Move on to the next format string */
        i_curr_format_string += 1;
        curr_format_string_start_index = i_char_base + 1; /* +1 so we can skip over the current newline character */
    }

    const uint32_t final_format_string_length = input_strlen - curr_format_string_start_index;
    format_string_lengths[i_curr_format_string] = final_format_string_length;

    klog_format_split_t result = {number_format_strings, format_strings, format_string_lengths};

    return result;
}

