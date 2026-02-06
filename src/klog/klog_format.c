#include "./klog_format.h"
#include "klog_platform.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_debug_util.h"
#include "./klog_platform.h"

const char* klog_format_input_message(const char* const s_format, va_list p_args) {
    /* We need to make a copy of the args (for the second vsnprintf call) before we consume them with the first vsnprintf call */
    va_list p_args_copy;
    va_copy(p_args_copy, p_args);

    /* Calculate the length of the input message */
    /* +1 for null termination */
    /* @todo kjk 2026/01/14 Use _vscprintf */
    const int input_message_length = vsnprintf(0, 0, s_format, p_args) + 1;

    /* Actually create the input message, now that we know the length */
    char* const s_input_message = malloc(input_message_length);

    /*  Format the input message with the unused copy of the args */
    vsnprintf(s_input_message, input_message_length, s_format, p_args_copy);
    va_end(p_args_copy);

    return s_input_message;
}

KlogFormatSplitInfo klog_format_split_strings(const char* const s_message) {
    const uint32_t input_strlen = strlen(s_message);

    uint32_t number_strings = 1;

    /* First pass to count how many strings we will have, so we can allocate our result buffers accordingly */
    for (uint32_t i_char_newline_count = 0; i_char_newline_count < input_strlen; ++i_char_newline_count) {
        if (s_message[i_char_newline_count] == '\n') {
            number_strings += 1;
        }
    }

    /* Allocate the resulting buffers */
    const char** const ls_strings = malloc(number_strings * sizeof(char*));
    uint32_t* const string_lengths = malloc(number_strings * sizeof(uint32_t));

    /* Initialize the first format string values in case we never reach a newline */
    ls_strings[0] = s_message;
    string_lengths[0] = input_strlen;

    /* Find all of the newlines so we know where the format string pointers should begin */
    uint32_t curr_format_string_start_index = 0;
    uint32_t i_curr_format_string = 0;
    for (uint32_t i_char_base = 0; i_char_base < input_strlen; ++i_char_base) {
        if (s_message[i_char_base] != '\n') {
            continue;
        }

        /* Update the information for our current format string */
        const uint32_t curr_format_string_length = i_char_base - curr_format_string_start_index;
        string_lengths[i_curr_format_string] = curr_format_string_length;
        if (i_curr_format_string < (number_strings - 1)) {
            ls_strings[i_curr_format_string+1] = s_message + i_char_base + 1;
        }

        /* Move on to the next format string */
        i_curr_format_string += 1;
        curr_format_string_start_index = i_char_base + 1; /* +1 so we can skip over the current newline character */
    }

    const uint32_t final_format_string_length = input_strlen - curr_format_string_start_index;
    string_lengths[i_curr_format_string] = final_format_string_length;

    const KlogFormatSplitInfo result = {number_strings, ls_strings, string_lengths};

    return result;
}

KlogString klog_format_time(void) {
    const timepoint_t timepoint = klog_platform_get_current_timepoint();

    /* Time prefix: DDD.HH:MM:SS:SSSSSS  */
    /* Length: 00+  123456789            */
    /*         10+           0123456789  */
    /*         20+                     0 */
    const uint32_t time_prefix_size = 19 + 1; /* +1 for null termination */
    char* const s_time = malloc(time_prefix_size);
    sprintf(s_time, "%.3d:%.2d:%.2d:%.2d:%.6d", timepoint.day_year, timepoint.hour, timepoint.minute, timepoint.second, timepoint.microsecond);

    KlogString packed_time = { time_prefix_size, s_time };
    return packed_time;
}

KlogString klog_format_source_location(const uint32_t filename_size_max, const char* const s_filepath, const uint32_t line_number) {
    /* filename, +1 for colon, +4 for line_number, +1 for null terminator */
    const uint32_t total_size = filename_size_max + 1 + 4 + 1;
    char* const s_formatted = malloc(total_size);

    /* Initialize with spaces, so the filename is padded correctly */
    memset(s_formatted, ' ', total_size);

    const char* const s_filename = klog_platform_get_basename(s_filepath);
    const uint32_t filename_size_original = strlen(s_filename);

    const uint32_t filename_size_copy = filename_size_max < filename_size_original ? filename_size_max : filename_size_original;
    memcpy(s_formatted, s_filename, filename_size_copy);

    /* Format the remainder after the filename has been populated */
    sprintf(s_formatted + filename_size_max, ":%4d", line_number);

    const KlogString packed_source_location = { total_size, s_formatted };

    return packed_source_location;
}
