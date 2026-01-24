#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __linux__
#else
#error "Only supporting linux"
#endif

#ifdef _WIN32
#error "Klog does not support Windows"
#endif

#ifdef __APPLE__
#error "Klog does not support MacOS"
#endif

#ifdef  __linux__

#include <unistd.h>      /* For pid_t */
#include <sys/syscall.h> /* For syscall() */
#include <time.h>        /* For time(), localtime(), gettimeofday() */
#include <sys/time.h>

#ifndef SYS_gettid
#error "SYS_gettid is unavailable on this system"
#endif

#endif

#include "./klog_debug_util.h"
#include "./klog_format.h"

pid_t klog_format_get_current_thread_id(void) {
    /* @todo kjk 2025/12/31 Call this once per thread so we can avoid switching into kernel mode repeatedly */
    return syscall(SYS_gettid);
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

KlogFormatSplitInfo klog_format_split_strings(const char* formatted_input) {
    const uint32_t input_strlen = strlen(formatted_input);

    uint32_t number_strings = 1;

    /* First pass to count how many format strings we will have, so we can allocate our result buffers accordingly */
    for (uint32_t i_char_newline_count = 0; i_char_newline_count < input_strlen; ++i_char_newline_count) {
        if (formatted_input[i_char_newline_count] == '\n') {
            number_strings += 1;
        }
    }

    /* Allocate the resulting buffers */
    const char** const strings = malloc(number_strings * sizeof(char*));
    uint32_t* const string_lengths = malloc(number_strings * sizeof(uint32_t));

    /* Initialize the first format string values in case we never reach a newline */
    strings[0] = formatted_input;
    string_lengths[0] = input_strlen;

    /* Find all of the newlines so we know where the format string pointers should begin */
    uint32_t curr_format_string_start_index = 0;
    uint32_t i_curr_format_string = 0;
    for (uint32_t i_char_base = 0; i_char_base < input_strlen; ++i_char_base) {
        if (formatted_input[i_char_base] != '\n') {
            continue;
        }

        /* Update the information for our current format string */
        const uint32_t curr_format_string_length = i_char_base - curr_format_string_start_index;
        string_lengths[i_curr_format_string] = curr_format_string_length;
        if (i_curr_format_string < (number_strings - 1)) {
            strings[i_curr_format_string+1] = formatted_input + i_char_base + 1;
        }

        /* Move on to the next format string */
        i_curr_format_string += 1;
        curr_format_string_start_index = i_char_base + 1; /* +1 so we can skip over the current newline character */
    }

    const uint32_t final_format_string_length = input_strlen - curr_format_string_start_index;
    string_lengths[i_curr_format_string] = final_format_string_length;

    KlogFormatSplitInfo result = {number_strings, strings, string_lengths};

    return result;
}

KlogString klog_format_time(void) {
    const time_t now = time(NULL);
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        kdprintf("Failure when invoking gettimeofday() for creation of filename\n");
        exit(1);
    }
    struct tm* p_broken_down_now = localtime(&now);
    const int32_t day = p_broken_down_now->tm_yday;
    const int32_t hour = p_broken_down_now->tm_hour;
    const int32_t minute = p_broken_down_now->tm_min;
    const int32_t second = p_broken_down_now->tm_sec;
    const uint32_t microsecond = tv.tv_usec;

    /* Time prefix: DDD.HH:MM:SS:SSSSSS  */
    /* Length: 00+  123456789            */
    /*         10+           0123456789  */
    /*         20+                     0 */
    const uint32_t time_prefix_size = 19 + 1; /* +1 for null termination */
    char* s_time = malloc(time_prefix_size);
    sprintf(s_time, "%.3d:%.2d:%.2d:%.2d:%.6d", day, hour, minute, second, microsecond);

    KlogString packed_time = { time_prefix_size, s_time };
    return packed_time;
}
