#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_constants.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"

bool klog_initialize_are_parameters_valid(const bool klog_is_initialized, const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length) {
    if (klog_is_initialized) {
        kdprintf("Trying to initialize klog, when it is already initialized\n");
        return false;
    }

    if (max_number_loggers == 0) {
        kdprintf("Trying to initialize klog with maximum number of loggers set to 0");
        return false;
    }

    if (logger_name_max_length == 0) {
        kdprintf("Trying to initialize klog with maximum length of logger names set to 0");
        return false;
    }

    if (message_queue_number_elements == 0) {
        kdprintf("Trying to initialize klog with maximum message queue size set to 0\n");
        return false;
    }

    if (message_max_length == 0) {
        kdprintf("Trying to initialize klog with maximum length of messages set to 0\n");
        return false;
    }

    return true;
}

char* klog_initialize_create_logger_names_buffer(const uint32_t max_number_loggers, const uint32_t logger_name_max_length) {
    /* set all characters to space (' ') by default, so we can auto fill the end of the names if they are too short */
    const uint32_t total_logger_names_array_size_bytes = max_number_loggers * logger_name_max_length;
    char* p_logger_names = malloc(total_logger_names_array_size_bytes);
    memset(p_logger_names, ' ', total_logger_names_array_size_bytes);

    kdprintf("Created logger names array\n");
    kdprintf("  start: %p\n", (void*)p_logger_names);
    kdprintf("  end  : %p\n", (void*)(p_logger_names + total_logger_names_array_size_bytes));
    kdprintf("  size : %d\n", total_logger_names_array_size_bytes);
    
    return p_logger_names;
}

uint8_t* klog_initialize_create_logger_levels_buffer(const uint32_t max_number_loggers) {
    const uint32_t logger_levels_array_size_bytes = max_number_loggers * sizeof(uint8_t*);
    uint8_t* p_logger_levels = malloc(logger_levels_array_size_bytes);
    memset(p_logger_levels, 0, logger_levels_array_size_bytes);
    
    kdprintf("Created logger levels array\n");
    kdprintf("  start: %p\n", (void*)p_logger_levels);
    kdprintf("  end  : %p\n", (void*)(p_logger_levels + (logger_levels_array_size_bytes * sizeof(uint8_t*))));
    kdprintf("  size : %d\n", logger_levels_array_size_bytes * sizeof(uint8_t*));
    
    return p_logger_levels;
}

char* klog_initialize_create_level_strings_buffer(void) {
    const uint32_t level_string_array_total_bytes = G_klog_level_string_length * G_klog_number_levels; 
    char* p_level_strings = malloc(level_string_array_total_bytes);
    const char off_string[]      = "off     ";
    const char critical_string[] = "critical";
    const char error_string[]    = "error   ";
    const char warning_string[]  = "warning ";
    const char info_string[]     = "info    ";
    const char debug_string[]    = "debug   ";
    const char trace_string[]    = "trace   ";
    memcpy(&p_level_strings[KLOG_LEVEL_OFF      * G_klog_level_string_length], off_string, strlen(off_string));
    memcpy(&p_level_strings[KLOG_LEVEL_CRITICAL * G_klog_level_string_length], critical_string, strlen(critical_string));
    memcpy(&p_level_strings[KLOG_LEVEL_ERROR    * G_klog_level_string_length], error_string, strlen(error_string));
    memcpy(&p_level_strings[KLOG_LEVEL_WARNING  * G_klog_level_string_length], warning_string, strlen(warning_string));
    memcpy(&p_level_strings[KLOG_LEVEL_INFO     * G_klog_level_string_length], info_string, strlen(info_string));
    memcpy(&p_level_strings[KLOG_LEVEL_DEBUG    * G_klog_level_string_length], debug_string, strlen(debug_string));
    memcpy(&p_level_strings[KLOG_LEVEL_TRACE    * G_klog_level_string_length], trace_string, strlen(trace_string));
    
    kdprintf("Created logger level strings array\n");
    kdprintf("  start: %p\n", (void*)p_level_strings);
    kdprintf("  end  : %p\n", (void*)(p_level_strings + level_string_array_total_bytes));
    kdprintf("  size : %d\n", level_string_array_total_bytes);
    
    return p_level_strings;
}

char* klog_initialize_create_message_queue(const uint32_t message_queue_number_elements, const uint32_t message_max_length) {
    const uint32_t message_queue_size_bytes = message_queue_number_elements * message_max_length;
    char* p_message_queue = malloc(message_queue_size_bytes);
    memset(p_message_queue, 0, message_queue_size_bytes);
    
    kdprintf("Created logger level strings array\n");
    kdprintf("  start: %p\n", (void*)p_message_queue);
    kdprintf("  end  : %p\n", (void*)(p_message_queue + message_queue_size_bytes));
    kdprintf("  size : %d\n", message_queue_size_bytes);
    
    return p_message_queue;
}

void klog_initialize_stdout(const klog_init_stdout_info_t* p_klog_init_stdout_info) {
    if (p_klog_init_stdout_info == NULL) {
        kdprintf("No stdout sink provided\n");
        return;
    }
}

void klog_initialize_file(const klog_init_file_info_t* p_klog_init_file_info) {
    if (p_klog_init_file_info == NULL) {
        kdprintf("No file sink provided\n");
        return;
    }
}

