#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_state.h"
#include "./klog_constants.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"
#include "./klog_output.h"
#include "./klog_format.h"

void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length, const uint32_t number_backing_threads, const KlogInitStdoutInfo* p_klog_init_stdout_info, const KlogInitFileInfo* p_klog_init_file_info) {
    if (!klog_initialize_are_parameters_valid(g_klog_is_initialized, max_number_loggers, logger_name_max_length, message_queue_number_elements, message_max_length)) {
        exit(1);
    }

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_logger_name_max_length = logger_name_max_length;

    gp_klog_logger_names = klog_initialize_create_logger_names_buffer(g_klog_max_number_loggers, g_klog_logger_name_max_length);
    kdprintf("gp_klog_logger_names: %p through %p\n", (void*)gp_klog_logger_names, (void*)(gp_klog_logger_names + (max_number_loggers * logger_name_max_length)));

    gp_klog_logger_levels = klog_initialize_create_logger_levels_buffer(g_klog_max_number_loggers);
    kdprintf("gp_klog_logger_levels: %p through %p\n", (void*)gp_klog_logger_levels, (void*)(gp_klog_logger_levels + g_klog_max_number_loggers));

    gp_klog_level_strings = klog_initialize_create_level_strings_buffer();
    kdprintf("gp_klog_level_strings: %p through %p\n", (void*)gp_klog_level_strings, (void*)(gp_klog_level_strings + (G_klog_level_string_length * G_klog_number_levels)));

    g_klog_current_number_loggers_created = 0;

    /* @todo kjk 2025/12/30 If we have no backing threads, then the queue size is 1, because it doesn't make sense to have mutliple items in the queue when we are blocking anyways */
    /* g_klog_message_queue_number_elements = message_queue_number_elements; */
    g_klog_message_queue_number_elements = 1;
    g_klog_message_max_length = message_max_length;
    gp_klog_message_queue = klog_initialize_create_message_queue(g_klog_message_queue_number_elements, g_klog_message_max_length);
    kdprintf("gp_klog_message_queue: %p through %p\n", (void*)gp_klog_message_queue, (void*)(gp_klog_message_queue + (g_klog_message_queue_number_elements * g_klog_message_max_length)));

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
        kdprintf("Trying to de-initialize klog, when it is not yet initialized\n");
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

KlogLoggerHandle klog_logger_create(const char* logger_name) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (g_klog_current_number_loggers_created >= g_klog_max_number_loggers) {
        kdprintf("Trying to create klog logger, but klog only allows %d loggers\n", g_klog_max_number_loggers);
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

    const KlogLoggerHandle handle = {current_logger_handle};
    return handle;
}

void klog_logger_set_level(const KlogLoggerHandle logger_handle, const enum klog_level_e updated_level) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        kdprintf("Trying to set level for logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    gp_klog_logger_levels[logger_handle.value] = updated_level;
}

void klog(const KlogLoggerHandle logger_handle, const enum klog_level_e requested_level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        kdprintf("Trying to log with logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    if (requested_level > gp_klog_logger_levels[logger_handle.value]) {
        return;
    }

    /* Create the input string with the arguments injected */
    va_list p_args;
    va_start(p_args, format);
    const char* const input_message = klog_format_input_message(format, p_args);
    va_end(p_args);

    /* Split the input string into multiple input strings based on the newlines */
    const KlogFormatSplitInfo split_messages_info = klog_format_split_strings(input_message);

    /* Get the information to create the message header */
    const pid_t thread_id = klog_format_get_current_thread_id();
    const char* logger_name = &(gp_klog_logger_names[logger_handle.value * g_klog_logger_name_max_length]);
    const char* level_string = &(gp_klog_level_strings[G_klog_level_string_length * requested_level]);

    /* For each input string:              */
    /* (using logger name of 6 characters) */
    /*      "123456 [ABCDEF] [12345678] "  */
    /* 00+   123456789                     */
    /* 10+            0123456789           */
    /* 20+                      01234567   */
    const uint32_t prefix_length = 21 + g_klog_logger_name_max_length;
    for (uint32_t i_message = 0; i_message < split_messages_info.number_strings; ++i_message) {
        /* Message size = header + input string + null terminator */
        const uint32_t current_message_length = split_messages_info.string_lengths[i_message]; /* Does this contain null terminator? */
        const uint32_t total_message_length = prefix_length + current_message_length + 1;
       
        /* Allocate space for the message and set the null terminator */
        char* total_message = malloc(total_message_length);
        total_message[total_message_length - 1] = 0;
        
        /* Populate the full message : header + input string + null terminator */
        sprintf(total_message, "%5d [%.*s] [%.*s] %.*s", thread_id, g_klog_logger_name_max_length, logger_name, G_klog_level_string_length, level_string, current_message_length, split_messages_info.strings[i_message]);

        /* Send the message on its merry way */
        klog_output_stdout(total_message);
        free(total_message);
    }

    free((char*)input_message);

    free(split_messages_info.strings);
    free((uint32_t*)split_messages_info.string_lengths);
}
