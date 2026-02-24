#include "./klog_initialize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_constants.h"
#include "./klog_handle.h"
#include "./klog_debug_util.h"
#include "./klog_platform.h"
#include "klog_format.h"

char* klog_initialize_buffer(const uint32_t number_elements, const uint32_t element_length_max, const char fill_char, const bool null_terminate) {
    const uint32_t element_length_max_real = null_terminate ? element_length_max + 1 : element_length_max;
    const uint32_t total_size = number_elements * element_length_max_real;
    char* const b_buffer = malloc(total_size);
    memset(b_buffer, fill_char, total_size);
    if (null_terminate) {
        for (uint32_t i = 0; i < number_elements; ++i) {
            const uint32_t end_index = (i * element_length_max_real) + (element_length_max_real - 1);
            b_buffer[end_index] = '\0';
        }
    }

    kdprintf("Created buffer\n");
    kdprintf("  start: %p\n", (void*)b_buffer);
    kdprintf("  end  : %p\n", (void*)(b_buffer + total_size));
    kdprintf("  total size : %d\n", total_size);
    kdprintf("  num elements: %d\n", number_elements);
    kdprintf("  element max length: %d\n", element_length_max);
    kdprintf("  null terminated elements: %b\n", null_terminate);
    
    return b_buffer;
}

bool klog_initialize_are_parameters_valid(const bool klog_is_initialized, const uint32_t max_number_loggers, const KlogFormatInfo klog_format_info, const KlogAsyncInfo* const p_klog_async_info, const KlogConsoleInfo* const p_klog_console_info, const KlogFileInfo* const p_klog_file_info) {
    if (klog_is_initialized) {
        kdprintf("Trying to initialize klog, when it is already initialized\n");
        return false;
    }

    if (max_number_loggers == 0) {
        kdprintf("Trying to initialize klog with maximum number of loggers set to 0");
        return false;
    }

    if (klog_format_info.logger_name_max_length == 0) {
        kdprintf("Trying to initialize klog with maximum length of logger names set to 0");
        return false;
    }

    if (klog_format_info.message_max_length == 0) {
        kdprintf("Trying to initialize klog with maximum length of messages set to 0\n");
        return false;
    }

    if (p_klog_async_info) {
        /* @todo kjk 2026/01/21 Validate async info */
        /* @todo kjk 2026/02/09 Add test to ut-klog_initialize_parmaeters_are_valid.c */
    }

    if (p_klog_file_info) {
        /* @todo kjk 2026/01/21 Validate file info */
        /* @todo kjk 2026/02/09 Add test to ut-klog_initialize_parmaeters_are_valid.c */
        /* Make sure filename prefix is valid */
    }

    /* @todo kjk 2026/01/21 This is currently unused */
    (void)p_klog_console_info;

    return true;
}

KlogLoggerHandle* klog_initialize_logger_handle_array(const uint32_t max_number_loggers) {
    const uint32_t total_handle_array_size = max_number_loggers * sizeof(KlogLoggerHandle);
    KlogLoggerHandle* const a_logger_handles = malloc(total_handle_array_size);
    kdprintf("Created logger handle array\n");
    kdprintf("  start: %p\n", (void*)a_logger_handles);
    kdprintf("  end  : %p\n", (void*)(a_logger_handles + total_handle_array_size));
    kdprintf("  size : %d\n", total_handle_array_size);

    return a_logger_handles;
}

uint8_t* klog_initialize_logger_levels_array(const uint32_t max_number_loggers) {
    const uint32_t logger_levels_array_size_bytes = max_number_loggers * sizeof(uint8_t*);
    uint8_t* const a_logger_levels = malloc(logger_levels_array_size_bytes);
    memset(a_logger_levels, 0, logger_levels_array_size_bytes);
    
    kdprintf("Created logger levels array\n");
    kdprintf("  start: %p\n", (void*)a_logger_levels);
    kdprintf("  end  : %p\n", (void*)(a_logger_levels + (logger_levels_array_size_bytes * sizeof(uint8_t*))));
    kdprintf("  size : %d\n", logger_levels_array_size_bytes * sizeof(uint8_t*));
    
    return a_logger_levels;
}

char* klog_initialize_level_strings_buffer(void) {
    const uint32_t level_string_array_total_bytes = G_klog_level_string_length * G_klog_number_levels; 
    char* const b_level_strings = malloc(level_string_array_total_bytes);
    const char off_string[]      = "off  ";
    const char fatal_string[]    = "FATAL";
    const char error_string[]    = "ERROR";
    const char warn_string[]     = "WARN ";
    const char info_string[]     = "info ";
    const char debug_string[]    = "debug";
    const char trace_string[]    = "trace";
    memcpy(&b_level_strings[KLOG_LEVEL_OFF      * G_klog_level_string_length], off_string, strlen(off_string));
    memcpy(&b_level_strings[KLOG_LEVEL_FATAL    * G_klog_level_string_length], fatal_string, strlen(fatal_string));
    memcpy(&b_level_strings[KLOG_LEVEL_ERROR    * G_klog_level_string_length], error_string, strlen(error_string));
    memcpy(&b_level_strings[KLOG_LEVEL_WARN     * G_klog_level_string_length], warn_string, strlen(warn_string));
    memcpy(&b_level_strings[KLOG_LEVEL_INFO     * G_klog_level_string_length], info_string, strlen(info_string));
    memcpy(&b_level_strings[KLOG_LEVEL_DEBUG    * G_klog_level_string_length], debug_string, strlen(debug_string));
    memcpy(&b_level_strings[KLOG_LEVEL_TRACE    * G_klog_level_string_length], trace_string, strlen(trace_string));
    
    kdprintf("Created logger level strings array\n");
    kdprintf("  start: %p\n", (void*)b_level_strings);
    kdprintf("  end  : %p\n", (void*)(b_level_strings + level_string_array_total_bytes));
    kdprintf("  size : %d\n", level_string_array_total_bytes);
    
    return b_level_strings;
}

char* klog_initialize_colored_level_strings_buffer(void) {
    const uint32_t colored_level_string_array_total_bytes = G_klog_colored_level_string_length * G_klog_number_levels; 
    char* const b_colored_level_strings = malloc(colored_level_string_array_total_bytes);
    const char off_string[]      = "\x1b[37moff  \x1b[0m"; /* white  */
    const char fatal_string[]    = "\x1b[41mFATAL\x1b[0m"; /* red BG */
    const char error_string[]    = "\x1b[31mERROR\x1b[0m"; /* red    */
    const char warn_string[]     = "\x1b[33mWARN \x1b[0m"; /* yellow */
    const char info_string[]     = "\x1b[32minfo \x1b[0m"; /* green  */
    const char debug_string[]    = "\x1b[36mdebug\x1b[0m"; /* cyan   */
    const char trace_string[]    = "\x1b[37mtrace\x1b[0m"; /* white  */
    memcpy(&b_colored_level_strings[KLOG_LEVEL_OFF      * G_klog_colored_level_string_length], off_string, strlen(off_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_FATAL    * G_klog_colored_level_string_length], fatal_string, strlen(fatal_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_ERROR    * G_klog_colored_level_string_length], error_string, strlen(error_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_WARN     * G_klog_colored_level_string_length], warn_string, strlen(warn_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_INFO     * G_klog_colored_level_string_length], info_string, strlen(info_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_DEBUG    * G_klog_colored_level_string_length], debug_string, strlen(debug_string));
    memcpy(&b_colored_level_strings[KLOG_LEVEL_TRACE    * G_klog_colored_level_string_length], trace_string, strlen(trace_string));
    
    kdprintf("Created colored logger level strings array\n");
    kdprintf("  start: %p\n", (void*)b_colored_level_strings);
    kdprintf("  end  : %p\n", (void*)(b_colored_level_strings + colored_level_string_array_total_bytes));
    kdprintf("  size : %d\n", colored_level_string_array_total_bytes);
    
    return b_colored_level_strings;
}

FILE* klog_initialize_file(const KlogFileInfo* const p_klog_file_info) {
    if (p_klog_file_info == NULL) {
        kdprintf("Not initializing output file\n");
        return NULL;
    }

    const timepoint_t timepoint = klog_platform_get_current_timepoint();

    const char* const s_sanitized_prefix = klog_format_file_name_prefix(p_klog_file_info->s_filename_prefix);

    /* Filename's are formatted like: <prefix>_YYYYMMDD_HHMMSS_SSSS.log */
    /* Extra chars                  : 00+     123456789                 */
    /*                                10+              0123456789       */
    /*                                20+                        012345 */
    const uint32_t prefix_length = strlen(s_sanitized_prefix);
    const uint32_t full_filename_length = prefix_length + 25 + 1; /* +1 for null terminator */
    char* const full_filename = malloc(full_filename_length);
    sprintf(full_filename, "%s_%.4d%.2d%.2d_%.2d%.2d%.2d_%.4d.log", s_sanitized_prefix, timepoint.year, timepoint.month, timepoint.day_month, timepoint.hour, timepoint.minute, timepoint.second, timepoint.microsecond/1000);

    FILE* const p_file = fopen(full_filename, "w");
    if (!p_file) {
        kdprintf("Failed to create log file at %s\n", full_filename);
        exit(KLOG_EXIT_CODE);
    }
    kdprintf("Created output file pointer at %p\n", (void*)p_file);

    free((char*)s_sanitized_prefix);
    free(full_filename);
    return p_file;
}

