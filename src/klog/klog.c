#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_impl_initialization.h"
#include "./klog_impl_message_creation.h"
#include "./klog_impl_state.h"

/* ================================================================================================================== */
/* Helper functions                                                                                                   */
/* ================================================================================================================== */

void klog_log_stdout(void) {
    if (g_klog_number_backing_threads != 0) {
        return;
    }

    /* @todo kjk 2025/12/30 Get the message from the queue and log it */
    printf("%s\n", gp_klog_message_queue);
}

void klog_log_file(void) {
    if (g_klog_number_backing_threads != 0) {
        return;
    }

    /* @todo kjk 2025/12/30 Get the message from the queue and log it */
}

/* ================================================================================================================== */
/* API Implementation                                                                                                 */
/* ================================================================================================================== */

void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length, const uint32_t number_backing_threads, const klog_init_stdout_info_t* p_klog_init_stdout_info, const klog_init_file_info_t* p_klog_init_file_info) {
    if (!klog_impl_parameters_are_valid(max_number_loggers, logger_name_max_length, message_queue_number_elements, message_max_length)) {
        exit(1);
    }

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_logger_name_max_length = logger_name_max_length;

    // /* set all characters to space (' ') by default, so we can auto fill the end of the names if they are too short */
    // const uint32_t total_logger_names_array_size = g_klog_logger_name_max_length * g_klog_max_number_loggers;
    // gp_klog_logger_names = malloc(total_logger_names_array_size);
    // memset(gp_klog_logger_names, ' ', total_logger_names_array_size);
    gp_klog_logger_names = klog_impl_create_logger_names_buffer(g_klog_max_number_loggers, g_klog_logger_name_max_length);

    // const uint32_t total_logger_levels_array_size = g_klog_max_number_loggers;
    // gp_klog_logger_levels = calloc(total_logger_levels_array_size, sizeof(gp_klog_logger_levels));
    gp_klog_logger_levels = klog_impl_create_logger_levels_buffer(g_klog_max_number_loggers);

    // const uint32_t level_string_array_total_bytes = G_klog_level_string_length * G_klog_number_levels; 
    // gp_klog_level_strings = malloc(level_string_array_total_bytes);
    // const char off_string[]      = "off     ";
    // const char critical_string[] = "critical";
    // const char error_string[]    = "error   ";
    // const char warning_string[]  = "warning ";
    // const char info_string[]     = "info    ";
    // const char debug_string[]    = "debug   ";
    // const char trace_string[]    = "trace   ";
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_OFF      * G_klog_level_string_length], off_string, strlen(off_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_CRITICAL * G_klog_level_string_length], critical_string, strlen(critical_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_ERROR    * G_klog_level_string_length], error_string, strlen(error_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_WARNING  * G_klog_level_string_length], warning_string, strlen(warning_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_INFO     * G_klog_level_string_length], info_string, strlen(info_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_DEBUG    * G_klog_level_string_length], debug_string, strlen(debug_string));
    // memcpy(&gp_klog_level_strings[KLOG_LEVEL_TRACE    * G_klog_level_string_length], trace_string, strlen(trace_string));
    gp_klog_level_strings = klog_impl_create_level_strings_buffer();

    g_klog_current_number_loggers_created = 0;

    /* @todo kjk 2025/12/30 If we have no backing threads, then the queue size is 1, because it doesn't make sense to have mutliple items in the queue when we are blocking anyways */
    /* g_klog_message_queue_number_elements = message_queue_number_elements; */
    g_klog_message_queue_number_elements = 1;
    g_klog_message_max_length = message_max_length;
    // const uint32_t message_queue_size_bytes = g_klog_message_queue_number_elements * g_klog_message_max_length;
    // gp_klog_message_queue = malloc(message_queue_size_bytes);
    // memset(gp_klog_message_queue, 0, message_queue_size_bytes);
    gp_klog_message_queue = klog_impl_create_message_queue(g_klog_message_queue_number_elements, g_klog_message_max_length);

    klog_impl_initialize_stdout(p_klog_init_stdout_info);
    klog_impl_initialize_file(p_klog_init_file_info);

    /* @todo kjk 2025/12/20 Create threads */
    /*  create backing threads accordingly. 0 is valid, meaning we always do IO in this thread */
    g_klog_number_backing_threads = number_backing_threads;

    g_klog_is_initialized = true;
}

void klog_deinitialize(void) {
    if (!g_klog_is_initialized) {
        printf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(1);
    }

    /* @todo kjk 2025/12/10 Join threads */

    g_klog_max_number_loggers = 0;
    g_klog_logger_name_max_length = 0;
    g_klog_current_number_loggers_created = 0;
    g_klog_message_queue_number_elements = 0;
    g_klog_message_max_length = 0;
    free(gp_klog_logger_names);
    free(gp_klog_logger_levels);
    free(gp_klog_level_strings);
    free(gp_klog_message_queue);
    g_klog_is_initialized = false;
}

klog_logger_handle_t klog_logger_create(const char* logger_name) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (g_klog_current_number_loggers_created >= g_klog_max_number_loggers) {
        printf("Trying to create klog logger, but klog only allows %d loggers\n", g_klog_max_number_loggers);
        exit(1);
    }

    const uint32_t current_logger_handle = g_klog_current_number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_handle * g_klog_logger_name_max_length;
    const uint32_t number_chars_to_copy = strlen(logger_name) >= g_klog_logger_name_max_length ?
        g_klog_logger_name_max_length : /* copy as much as we can fit */
        strlen(logger_name);            /* copy it all - NOT including the null terminator (which strlen doesn't count anyways) */
    memcpy(&gp_klog_logger_names[logger_name_start_index], logger_name, number_chars_to_copy);

    gp_klog_logger_levels[current_logger_handle] = KLOG_LEVEL_OFF;

    g_klog_current_number_loggers_created++;

    const klog_logger_handle_t handle = {current_logger_handle};
    return handle;
}

void klog_logger_set_level(const klog_logger_handle_t logger_handle, const enum klog_level_e updated_level) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        printf("Trying to set level for logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    gp_klog_logger_levels[logger_handle.value] = updated_level;
}

void klog(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        printf("Trying to log with logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    if (requested_level > gp_klog_logger_levels[logger_handle.value]) {
        return;
    }

    const pid_t thread_id = klog_impl_get_current_thread_id();
    const char* logger_name = &gp_klog_logger_names[logger_handle.value * g_klog_logger_name_max_length];
    const char* level_string = klog_impl_get_level_string(requested_level);
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

    va_list args;
    va_start(args, format);
    /* +1 for null termination */
    const int input_message_length = vsnprintf(0, 0, format, args) + 1;
    va_end(args);
    char* input_message = malloc(input_message_length);
    va_start(args, format);
    vsnprintf(input_message, input_message_length, format, args);
    va_end(args);

    const uint32_t prefix_length = 20 + g_klog_logger_name_max_length;
    /* const uint32_t total_message_length = (prefix_length + input_message_length) < g_klog_message_max_length ?
        prefix_length + input_message_length :
        g_klog_message_max_length; */
    const uint32_t total_message_length = prefix_length + input_message_length;
    char* total_message = malloc(total_message_length);
    sprintf(total_message, "%5d [%.*s] [%.*s] %s", thread_id, g_klog_logger_name_max_length, logger_name, G_klog_level_string_length, level_string, input_message);
    printf("%s\n", total_message);

    // /* @todo kjk 2025/12/30 Write to the buffer */
    // memcpy(gp_klog_message_queue, total_message, total_message_length);

    // free(input_message);
    // free(total_message);

    // if (g_klog_number_backing_threads > 0) {
    //     return;
    // }
    // klog_log_stdout();
    // klog_log_file();
    // /* @todo kjk 2025/12/30 Account for multiple messages in the queue when consuming */
    // memset(gp_klog_message_queue, 0, total_message_length);
}
