#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __linux__
#include <time.h>        /* For time(), localtime(), gettimeofday() */
#include <sys/time.h>
#else
#error "Only supporting linux"
#endif

#include "./klog_constants.h"
#include "./klog_handle.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"

bool klog_initialize_are_parameters_valid(const bool klog_is_initialized, const uint32_t max_number_loggers, const KlogFormatInfo klog_format_info, const KlogAsyncInfo* p_klog_async_info, const KlogStdoutInfo* p_klog_init_stdout_info, const KlogFileInfo* p_klog_init_file_info) {
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
    }

    if (p_klog_init_file_info) {
        /* @todo kjk 2026/01/21 Validate file info */
        /* Make sure filename prefix is valid */
    }

    /* @todo kjk 2026/01/21 This is currently unused */
    (void)p_klog_init_stdout_info;

    return true;
}

KlogLoggerHandle* klog_initialize_logger_handle_array(const uint32_t max_number_loggers) {
    const uint32_t total_handle_array_size = max_number_loggers * sizeof(KlogLoggerHandle);
    KlogLoggerHandle* a_logger_handles = malloc(total_handle_array_size);
    kdprintf("Created logger handle array\n");
    kdprintf("  start: %p\n", (void*)a_logger_handles);
    kdprintf("  end  : %p\n", (void*)(a_logger_handles + total_handle_array_size));
    kdprintf("  size : %d\n", total_handle_array_size);

    return a_logger_handles;
}

char* klog_initialize_logger_names_buffer(const uint32_t max_number_loggers, const uint32_t logger_name_max_length) {
    /* set all characters to space (' ') by default, so we can auto pad the end of the names if they are too short */
    const uint32_t total_logger_names_array_size_bytes = max_number_loggers * logger_name_max_length;
    char* b_logger_names = malloc(total_logger_names_array_size_bytes);
    memset(b_logger_names, ' ', total_logger_names_array_size_bytes);

    kdprintf("Created logger names array\n");
    kdprintf("  start: %p\n", (void*)b_logger_names);
    kdprintf("  end  : %p\n", (void*)(b_logger_names + total_logger_names_array_size_bytes));
    kdprintf("  size : %d\n", total_logger_names_array_size_bytes);
    
    return b_logger_names;
}

uint8_t* klog_initialize_logger_levels_array(const uint32_t max_number_loggers) {
    const uint32_t logger_levels_array_size_bytes = max_number_loggers * sizeof(uint8_t*);
    uint8_t* a_logger_levels = malloc(logger_levels_array_size_bytes);
    memset(a_logger_levels, 0, logger_levels_array_size_bytes);
    
    kdprintf("Created logger levels array\n");
    kdprintf("  start: %p\n", (void*)a_logger_levels);
    kdprintf("  end  : %p\n", (void*)(a_logger_levels + (logger_levels_array_size_bytes * sizeof(uint8_t*))));
    kdprintf("  size : %d\n", logger_levels_array_size_bytes * sizeof(uint8_t*));
    
    return a_logger_levels;
}

char* klog_initialize_level_strings_buffer(void) {
    const uint32_t level_string_array_total_bytes = G_klog_level_string_length * G_klog_number_levels; 
    char* b_level_strings = malloc(level_string_array_total_bytes);
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
    char* b_colored_level_strings = malloc(colored_level_string_array_total_bytes);
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

char* klog_initialize_message_queue(const uint32_t message_queue_number_elements, const uint32_t message_max_length) {
    const uint32_t message_queue_size_bytes = message_queue_number_elements * message_max_length;
    char* b_message_queue = malloc(message_queue_size_bytes);
    memset(b_message_queue, 0, message_queue_size_bytes);
    
    kdprintf("Created logger level strings array\n");
    kdprintf("  start: %p\n", (void*)b_message_queue);
    kdprintf("  end  : %p\n", (void*)(b_message_queue + message_queue_size_bytes));
    kdprintf("  size : %d\n", message_queue_size_bytes);
    
    return b_message_queue;
}

FILE* klog_initialize_file(const KlogFileInfo* p_klog_init_file_info) {
    if (p_klog_init_file_info == NULL) {
        kdprintf("Not initializing output file\n");
        return NULL;
    }

    const time_t now = time(NULL);
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        kdprintf("Failure when invoking gettimeofday() for creation of filename\n");
        exit(1);
    }
    struct tm* p_broken_down_now = localtime(&now);
    const int32_t year = p_broken_down_now->tm_year + 1900;
    const int32_t month = p_broken_down_now->tm_mon;
    const int32_t day = p_broken_down_now->tm_mday;
    const int32_t hour = p_broken_down_now->tm_hour;
    const int32_t minute = p_broken_down_now->tm_min;
    const int32_t second = p_broken_down_now->tm_sec;
    const uint32_t millisecond = tv.tv_usec / 1000;

    /* Filename's are formatted like: <prefix>_YYYYMMDD_HHMMSS_SSSS.log */
    /* Extra chars                  : 00+     123456789                 */
    /*                                10+              0123456789       */
    /*                                20+                        012345 */
    const uint32_t prefix_length = strlen(p_klog_init_file_info->filename_prefix);
    const uint32_t full_filename_length = prefix_length + 25 + 1; /* +1 for null terminator */
    char* full_filename = malloc(full_filename_length);
    sprintf(full_filename, "%s_%.4d%.2d%.2d_%.2d%.2d%.2d_%.4d.log", p_klog_init_file_info->filename_prefix, year, month, day, hour, minute, second, millisecond);

    FILE* p_file = fopen(full_filename, "w");
    if (!p_file) {
        kdprintf("Failed to create log file at %s\n", full_filename);
        exit(1);
    }
    kdprintf("Created output file pointer at %p\n", (void*)p_file);

    free(full_filename);
    return p_file;
}

