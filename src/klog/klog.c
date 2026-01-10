#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_state.h"
#include "./klog_initialize.h"
#include "./klog_output.h"
#include "./klog_format.h"

void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length, const uint32_t number_backing_threads, const klog_init_stdout_info_t* p_klog_init_stdout_info, const klog_init_file_info_t* p_klog_init_file_info) {
    if (!klog_initialize_are_parameters_valid(g_klog_is_initialized, max_number_loggers, logger_name_max_length, message_queue_number_elements, message_max_length)) {
        exit(1);
    }

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_logger_name_max_length = logger_name_max_length;

    gp_klog_logger_names = klog_initialize_create_logger_names_buffer(g_klog_max_number_loggers, g_klog_logger_name_max_length);

    gp_klog_logger_levels = klog_initialize_create_logger_levels_buffer(g_klog_max_number_loggers);

    gp_klog_level_strings = klog_initialize_create_level_strings_buffer();

    g_klog_current_number_loggers_created = 0;

    /* @todo kjk 2025/12/30 If we have no backing threads, then the queue size is 1, because it doesn't make sense to have mutliple items in the queue when we are blocking anyways */
    /* g_klog_message_queue_number_elements = message_queue_number_elements; */
    g_klog_message_queue_number_elements = 1;
    g_klog_message_max_length = message_max_length;
    gp_klog_message_queue = klog_initialize_create_message_queue(g_klog_message_queue_number_elements, g_klog_message_max_length);

    klog_initialize_stdout(p_klog_init_stdout_info);
    klog_initialize_file(p_klog_init_file_info);

    /* @todo kjk 2025/12/20 Create threads */
    /*  create backing threads accordingly. 0 is valid, meaning we always do IO in this thread */
    g_klog_number_backing_threads = number_backing_threads;
    g_klog_number_backing_threads = 0;

    g_klog_is_initialized = true;
}

void klog_deinitialize(void) {
    if (!g_klog_is_initialized) {
        printf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(1);
    }

    g_klog_max_number_loggers = 0;
    g_klog_logger_name_max_length = 0;
    g_klog_number_backing_threads = 0;
    g_klog_message_queue_number_elements = 0;
    g_klog_message_max_length = 0;

    g_klog_current_number_loggers_created = 0;
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

    /**
     * @todo kjk 2025/12/31 The logic for this function should look like this:
     *      1. Split the format strings
     *      2. Create the corresponding messages
     *      3. For each resultant message:
     *          If threaded:
     *              put the message into the message queue
     *          Else:
     *              log the message
     *          Free the message
     */

    /* Split the format string into multiple format strings */

    /* Iterate over every format string */
        /* Get the correct args based on the information for the current format string */
        /* Produce the message for the corresponding format string and args */

    va_list args;
    va_start(args, format);
    const klog_format_context_t format_context;
    char* total_message = klog_format(format_context, logger_handle, requested_level, format, args);
    va_end(args);

    if (g_klog_number_backing_threads > 0) {
        /* @todo kjk 2025/12/31 If multiple threads, put these in the thread queue and return */
        free(total_message);
        return;
    }

    klog_output_stdout(total_message);

    free(total_message);
}
