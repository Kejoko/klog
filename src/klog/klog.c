#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_state.h"
#include "./klog_constants.h"
#include "./klog_handle.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"
#include "./klog_output.h"
#include "./klog_format.h"

void klog_initialize(const uint32_t max_number_loggers, const KlogFormatInfo klog_format_info, const KlogAsyncInfo* p_klog_async_info, const KlogStdoutInfo* p_klog_init_stdout_info, const KlogFileInfo* p_klog_init_file_info) {
    if (!klog_initialize_are_parameters_valid(g_klog_is_initialized, max_number_loggers, klog_format_info, p_klog_async_info, p_klog_init_stdout_info, p_klog_init_file_info)) {
        exit(1);
    }

    /* @todo kjk 2026/01/21 Validate async parameters */

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_logger_name_max_length = klog_format_info.logger_name_max_length;

    gp_klog_logger_names = klog_initialize_logger_names_buffer(g_klog_max_number_loggers, g_klog_logger_name_max_length);
    kdprintf("gp_klog_logger_names: %p through %p\n", (void*)gp_klog_logger_names, (void*)(gp_klog_logger_names + (max_number_loggers * klog_format_info.logger_name_max_length)));

    ga_klog_logger_levels = klog_initialize_logger_levels_array(g_klog_max_number_loggers);
    kdprintf("ga_klog_logger_levels: %p through %p\n", (void*)ga_klog_logger_levels, (void*)(ga_klog_logger_levels + g_klog_max_number_loggers));

    gb_klog_level_strings = klog_initialize_level_strings_buffer();
    kdprintf("gb_klog_level_strings: %p through %p\n", (void*)gb_klog_level_strings, (void*)(gb_klog_level_strings + (G_klog_level_string_length * G_klog_number_levels)));

    g_klog_current_number_loggers_created = 0;

    /* @todo Move this into the klog initialization function */
    ga_klog_logger_handles = klog_initialize_logger_handle_array(g_klog_max_number_loggers);
    kdprintf("ga_klog_logger_handles: %p through %p\n", (void*)ga_klog_logger_handles, (void*)(ga_klog_logger_handles + (g_klog_max_number_loggers * sizeof(KlogLoggerHandle))));

    g_klog_message_max_length = klog_format_info.message_max_length;

    /* @todo kjk 2025/12/30 If we have no backing threads, then the queue size is 1, because it doesn't make sense to have mutliple items in the queue when we are blocking anyways */
    /* @todo kjk 2025/12/20 Create threads */
    g_klog_number_backing_threads = 0;
    g_klog_message_queue_number_elements = 0;
    if (p_klog_async_info) {
        g_klog_number_backing_threads = p_klog_async_info->number_backing_threads;

        gb_klog_message_queue = klog_initialize_message_queue(g_klog_message_queue_number_elements, g_klog_message_max_length);
        kdprintf("gb_klog_message_queue: %p through %p\n", (void*)gb_klog_message_queue, (void*)(gb_klog_message_queue + (g_klog_message_queue_number_elements * g_klog_message_max_length)));
    }

    /* @todo kjk 2026/01/21 Save and enforce the logging levels for each of the sinks */
    gp_klog_output_file = klog_initialize_file(p_klog_init_file_info);
    kdprintf("gp_klog_output_file: %p\n", (void*)gp_klog_output_file);

    g_klog_print_timestamp = klog_format_info.use_timestamp;
    g_klog_print_source_location = klog_format_info.use_source_location;

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
    g_klog_print_timestamp = false;
    g_klog_print_source_location = false;
    g_klog_message_max_length = 0;

    g_klog_current_number_loggers_created = 0;
    free(ga_klog_logger_handles);
    free(gp_klog_logger_names);
    free(ga_klog_logger_levels);
    free(gb_klog_level_strings);
    free(gb_klog_message_queue);
   
    if (gp_klog_output_file) {
        fclose(gp_klog_output_file);
    }
    
    g_klog_is_initialized = false;
}

KlogLoggerHandle* klog_logger_create(const char* logger_name) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (g_klog_current_number_loggers_created >= g_klog_max_number_loggers) {
        kdprintf("Trying to create klog logger, but klog only allows %d loggers\n", g_klog_max_number_loggers);
        exit(1);
    }

    const uint32_t current_logger_index = g_klog_current_number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_index * g_klog_logger_name_max_length;
    const uint32_t number_chars_to_copy = strlen(logger_name) >= g_klog_logger_name_max_length ?
        g_klog_logger_name_max_length : /* copy as much as we can fit */
        strlen(logger_name);            /* copy it all - NOT including the null terminator (which strlen doesn't count anyways) */
    memcpy(&gp_klog_logger_names[logger_name_start_index], logger_name, number_chars_to_copy);

    ga_klog_logger_levels[current_logger_index] = KLOG_LEVEL_OFF;

    /* We have to do this weird casting stuff to prevent valgrind from exploding */
    KlogLoggerHandle* p_logger_handle = &(((KlogLoggerHandle*)ga_klog_logger_handles)[current_logger_index]);
    p_logger_handle->value = current_logger_index;

    g_klog_current_number_loggers_created++;

    return p_logger_handle;
}

void klog_logger_set_level(const KlogLoggerHandle* p_logger_handle, const enum klog_level_e updated_level) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (p_logger_handle->value >= g_klog_current_number_loggers_created) {
        kdprintf("Trying to set level for logger %d, when only %d loggers exist\n", p_logger_handle->value, g_klog_current_number_loggers_created);
        exit(1);
    }

    ga_klog_logger_levels[p_logger_handle->value] = updated_level;
}

void klog(const KlogLoggerHandle* p_logger_handle, const enum klog_level_e requested_level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (p_logger_handle->value >= g_klog_current_number_loggers_created) {
        kdprintf("Trying to log with logger %d, when only %d loggers exist\n", p_logger_handle->value, g_klog_current_number_loggers_created);
        exit(1);
    }

    if (requested_level > ga_klog_logger_levels[p_logger_handle->value]) {
        return;
    }

    /* We are getting the time first, so it's closest to the actual point of invocation */
    KlogString packed_time;
    KlogString* p_packed_time = NULL;
    if (g_klog_print_timestamp) {
        packed_time = klog_format_time();
        p_packed_time = &packed_time;
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
    const char* s_logger_name = &(gp_klog_logger_names[p_logger_handle->value * g_klog_logger_name_max_length]);
    const char* s_level = &(gb_klog_level_strings[G_klog_level_string_length * requested_level]);

    KlogString packed_name = {g_klog_logger_name_max_length, s_logger_name};
    KlogString packed_level = {G_klog_level_string_length, s_level};

    KlogString packed_source_location;
    KlogString* p_packed_source_location = NULL;
    if (g_klog_print_source_location) {
        /* @todo Create the source location string */
        p_packed_source_location = &packed_source_location;
    }

    /* Thread ID prefix: "123456 " */
    /* Time prefix:      "DDD.HH:MM:SS:SSSS " */
    /* Name prefix:      "[ABCDEF] "*/
    /* Level prefix:     "[ABCDE] " */
    /* Source prefix:    "[abcdefghijklmnopqrst:XXXX] " */
    for (uint32_t i_message = 0; i_message < split_messages_info.number_strings; ++i_message) {
        KlogString packed_message = {split_messages_info.string_lengths[i_message], split_messages_info.strings[i_message]};

        klog_output_stdout(thread_id, p_packed_time, &packed_name, &packed_level, p_packed_source_location, &packed_message);
        if (gp_klog_output_file) {
            klog_output_file(gp_klog_output_file, thread_id, p_packed_time, &packed_name, &packed_level, p_packed_source_location, &packed_message);
        }
    }

    free((char*)input_message);

    free(split_messages_info.strings);
    free((uint32_t*)split_messages_info.string_lengths);

    if (g_klog_print_timestamp) {
        free((char*)packed_time.s);
    }

    if (g_klog_print_source_location) {
        free((char*)packed_source_location.s);
    }
}
