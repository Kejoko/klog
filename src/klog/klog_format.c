#include "./klog_format.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./klog_constants.h"
#include "./klog_debug_util.h"
#include "./klog_platform.h"

uint32_t klog_format_prefix_length_get(const bool use_thread_id, const bool use_timestamp, const uint32_t logger_name_max_length, const bool use_color, const uint32_t source_location_filename_max_length) {
    uint32_t total = 0;

    if (use_thread_id) {
        total += 7 + 1; /* 7 digit id + space */
    }
    if (use_timestamp) {
        total += G_klog_time_string_length + 1; /* 19 digit timestamp + space */
    }
    total += logger_name_max_length + 2 + 1; /* logger name + brackets + space */
    total += G_klog_level_string_length + 2 + 1; /* level + brackets + space */
    if (use_color) {
        total += 9;
    }
    if (source_location_filename_max_length > 0) {
        total += source_location_filename_max_length + 2 + 1 + 4 + 1; /* filename + brackets + colon + line + space */
    }

    return total;
}

const char* klog_format_logger_name(const char* const s_name) {
    /**
     * Perhaps we should be doing this in place but creating loggers is not where we will
     * be saving on performance, so mallocing a copy is okay
     */

    const uint32_t length_name = strlen(s_name);
    char* s_sanitized_name = malloc(length_name + 1); /* +1 for null termination */

    for (uint32_t i_input_char = 0; i_input_char < length_name; ++i_input_char) {
        const char curr_char = s_name[i_input_char];
        if (curr_char == '\n' || curr_char == '\t' || curr_char == ' ' || curr_char == '\r' || curr_char == '\b' || curr_char == '\0') {
            s_sanitized_name[i_input_char] = '_';
            continue;
        }

        s_sanitized_name[i_input_char] = curr_char;
    }

    s_sanitized_name[length_name] = '\0';

    return s_sanitized_name;
}

const char* klog_format_file_name_prefix(const char* const s_name) {
    /**
     * Perhaps we should be doing this in place but creating the file only happens once so, oh well
     */

    /* @todo kjk 2026/02/12 Should we be doing anything to sanitize filepaths for windows vs linux? Convert "/" to "\\" and vice versa?? */

    return klog_format_logger_name(s_name);
}

KlogString klog_format_message_prefix(char* s_prefix, const uint32_t* const p_thread_id, const KlogString* const p_time, const KlogString* const p_name, const KlogString* const p_level, const KlogString* const p_source_location) {
    /*         "0062503 043:08:14:31:933041 [ABC   ] [debug] [ft-klog_ba:  35] " */
    /*          |                           |        |       |                   */
    /* 00+      12345678|                   |        |       |                   */
    /* 00+              123456789           |        |       |                   */
    /* 10+                       0123456789 |        |       |                   */
    /* 20+                                 0|        |       |                   */
    /* 00+                                  123456789|       |                   */
    /* 00+                                           12345678|                   */
    
    /**
     * @brief So in total we have:
     *      8[thread id and space] +
     *      (p_time.length)[timestamp and space] +
     *      (p_name.length+3)[logger name, brackets, space] +
     *      (p_level+3)[level name, brackets, space] +
     *      (p_source_location.length+3)[source location, brackets, space]
     */

    /* @todo should we use our prefix_length_get function here? */
    /* @todo should we memset to 0 at the front here instead of the end of klog_log? */

    uint32_t size_total = 0;
    if (p_thread_id) {
        size_total = size_total + 8;
    }
    if (p_time && p_time->length) {
        size_total = size_total + p_time->length + 1;
    }
    if (p_name && p_name->length) {
        size_total = size_total + p_name->length + 3;
    }
    if (p_level && p_level->length) {
        size_total = size_total + p_level->length + 3;
    }
    if (p_source_location && p_source_location->length) {
        size_total = size_total + p_source_location->length + 3;
    }

    if (size_total == 0) {
        return (KlogString){0, s_prefix};
    }

    uint32_t write_offset = 0;
    if (p_thread_id) {
        sprintf(&(s_prefix[write_offset]), "%.7d ", *p_thread_id);
        write_offset = write_offset + 8;
    }
    if (p_time && p_time->length) {
        sprintf(&(s_prefix[write_offset]), "%.*s ", p_time->length, p_time->s);
        write_offset = write_offset + p_time->length + 1;
    }
    if (p_name && p_name->length) {
        sprintf(&(s_prefix[write_offset]), "[%.*s] ", p_name->length, p_name->s);
        write_offset = write_offset + p_name->length + 3;
    }
    if (p_level && p_level->length) {
        sprintf(&(s_prefix[write_offset]), "[%.*s] ", p_level->length, p_level->s);
        write_offset = write_offset + p_level->length + 3;
    }
    if (p_source_location && p_source_location->length) {
        sprintf(&(s_prefix[write_offset]), "[%.*s] ", p_source_location->length, p_source_location->s);
        write_offset = write_offset + p_source_location->length + 3;
    }

    /* Set the final byte to the null terminator */
    s_prefix[size_total] = '\0';

    return (KlogString){size_total, s_prefix};
}

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

KlogString klog_format_time(char* b_time) {
    const timepoint_t timepoint = klog_platform_get_current_timepoint();

    /* Time prefix: DDD:HH:MM:SS:SSSSSS */
    /* Length: 00+  123456789           */
    /*         10+           0123456789 */
    sprintf(b_time, "%.3d:%.2d:%.2d:%.2d:%.6d", timepoint.day_year, timepoint.hour, timepoint.minute, timepoint.second, timepoint.microsecond);

    KlogString packed_time = { G_klog_time_string_length, b_time };
    return packed_time;
}

KlogString klog_format_source_location(const uint32_t filename_size_max, const char* const s_filepath, const uint32_t line_number) {
    if (filename_size_max == 0) {
        return (KlogString){0, NULL};
    }

    /* filename, +1 for colon, +4 for line_number */
    const uint32_t total_size = filename_size_max + 1 + 4;
    char* const s_formatted = malloc(total_size + 1); /* +1 for null terminator */

    /* Initialize with spaces, so the filename is padded correctly */
    memset(s_formatted, ' ', total_size);

    const char* const s_filename = klog_platform_get_basename(s_filepath);
    const uint32_t filename_size_original = strlen(s_filename);

    const uint32_t filename_size_copy = filename_size_max < filename_size_original ? filename_size_max : filename_size_original;
    memcpy(s_formatted, s_filename, filename_size_copy);

    /* Format the remainder after the filename has been populated */
    const uint32_t line_number_adjusted = line_number <= 9999 ? line_number : 9999;
    sprintf(s_formatted + filename_size_max, ":%4d", line_number_adjusted);

    /* We are not reporting the null terminator in our length */
    const KlogString packed_source_location = { total_size, s_formatted };

    return packed_source_location;
}
