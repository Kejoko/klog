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
#include "./klog_platform.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"
#include "./klog_output.h"
#include "./klog_format.h"

void klog_initialize(const uint32_t max_number_loggers, const KlogFormatInfo klog_format_info, const KlogAsyncInfo* const p_klog_async_info, const KlogConsoleInfo* const p_klog_console_info, const KlogFileInfo* const p_klog_file_info) {
#ifdef KLOG_OFF
    (void)max_number_loggers;
    (void)klog_format_info;
    (void)p_klog_async_info;
    (void)p_klog_console_info;
    (void)p_klog_file_info;
#else
    if (!klog_initialize_are_parameters_valid(g_klog_state.is_initialized, max_number_loggers, klog_format_info, p_klog_async_info, p_klog_console_info, p_klog_file_info)) {
        exit(1);
    }

    g_klog_config.format = klog_format_info;
    if (p_klog_async_info) {
        g_klog_config.async = *p_klog_async_info;
    }
    if (p_klog_console_info) {
        g_klog_config.console = *p_klog_console_info;
    }
    if (p_klog_file_info) {
        g_klog_config.file = *p_klog_file_info;
    }

    g_klog_state.number_loggers_max = max_number_loggers;

    g_klog_state.b_logger_names = klog_initialize_logger_names_buffer(g_klog_state.number_loggers_max, g_klog_config.format.logger_name_max_length);
    kdprintf("b_logger_names: %p through %p\n", (void*)g_klog_state.b_logger_names, (void*)(g_klog_state.b_logger_names + (max_number_loggers * klog_format_info.logger_name_max_length)));

    g_klog_state.a_logger_levels = klog_initialize_logger_levels_array(g_klog_state.number_loggers_max);
    kdprintf("a_logger_levels: %p through %p\n", (void*)g_klog_state.a_logger_levels, (void*)(g_klog_state.a_logger_levels + g_klog_state.number_loggers_max));

    g_klog_state.b_level_strings = klog_initialize_level_strings_buffer();
    kdprintf("b_level_strings: %p through %p\n", (void*)g_klog_state.b_level_strings, (void*)(g_klog_state.b_level_strings + (G_klog_level_string_length * G_klog_number_levels)));

    g_klog_state.b_level_strings_colored = klog_initialize_colored_level_strings_buffer();
    kdprintf("b_level_strings_colored: %p through %p\n", (void*)g_klog_state.b_level_strings_colored, (void*)(g_klog_state.b_level_strings_colored + (G_klog_colored_level_string_length * G_klog_number_levels)));

    g_klog_state.number_loggers_created = 0;

    g_klog_state.a_logger_handles = klog_initialize_logger_handle_array(g_klog_state.number_loggers_max);
    kdprintf("a_logger_handles: %p through %p\n", (void*)g_klog_state.a_logger_handles, (void*)(g_klog_state.a_logger_handles + (g_klog_state.number_loggers_max * sizeof(KlogLoggerHandle))));

    /* @todo kjk 2025/12/30 If we have no backing threads, then the queue size is 1, because it doesn't make sense to have mutliple items in the queue when we are blocking anyways */
    /* @todo kjk 2025/12/20 Create threads */
    if (p_klog_async_info) {
        g_klog_state.b_message_queue = klog_initialize_message_queue(g_klog_config.async.message_queue_number_elements, g_klog_config.format.message_max_length);
        kdprintf("b_message_queue: %p through %p\n", (void*)g_klog_state.b_message_queue, (void*)(g_klog_state.b_message_queue + (g_klog_config.async.message_queue_number_elements * g_klog_config.format.message_max_length)));
    }

    g_klog_state.p_file = klog_initialize_file(p_klog_file_info);
    kdprintf("p_file: %p\n", (void*)g_klog_state.p_file);
    kdprintf("File max verbosity: %d\n", g_klog_config.file.max_level);

    g_klog_state.is_initialized = true;
#endif
}

void klog_deinitialize(void) {
#ifdef KLOG_OFF
#else
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(1);
    }

    g_klog_config = (struct KlogConfig){ 0 };

    g_klog_state.number_loggers_max = 0;

    g_klog_state.number_loggers_created = 0;
    free(g_klog_state.a_logger_handles);
    free(g_klog_state.b_logger_names);
    free(g_klog_state.a_logger_levels);
    free(g_klog_state.b_level_strings);
    free(g_klog_state.b_level_strings_colored);
    free(g_klog_state.b_message_queue);

    if (g_klog_state.p_file) {
        fclose(g_klog_state.p_file);
    }

    g_klog_state.is_initialized = false;
#endif
}

const KlogLoggerHandle* klog_logger_create(const char* const s_logger_name) {
#ifdef KLOG_OFF
    (void)logger_name;
    return NULL;
#else
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (g_klog_state.number_loggers_created >= g_klog_state.number_loggers_max) {
        kdprintf("Trying to create klog logger, but klog only allows %d loggers\n", g_klog_state.number_loggers_max);
        exit(1);
    }

    const uint32_t current_logger_index = g_klog_state.number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_index * g_klog_config.format.logger_name_max_length;
    const uint32_t number_chars_to_copy = strlen(s_logger_name) >= g_klog_config.format.logger_name_max_length ?
        g_klog_config.format.logger_name_max_length : /* copy as much as we can fit */
        strlen(s_logger_name);                             /* copy it all - NOT including the null terminator (which strlen doesn't count anyways) */
    memcpy(&g_klog_state.b_logger_names[logger_name_start_index], s_logger_name, number_chars_to_copy);

    g_klog_state.a_logger_levels[current_logger_index] = KLOG_LEVEL_OFF;

    /* We have to do this weird casting stuff to prevent valgrind from exploding */
    KlogLoggerHandle* const p_logger_handle = &(((KlogLoggerHandle*)g_klog_state.a_logger_handles)[current_logger_index]);
    p_logger_handle->value = current_logger_index;

    g_klog_state.number_loggers_created++;

    return p_logger_handle;
#endif
}

void klog_logger_set_level(const KlogLoggerHandle* const p_logger_handle, const enum KlogLevel updated_level) {
#ifdef KLOG_OFF
    (void)p_logger_handle;
    (void)updated_level;
#else
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (p_logger_handle->value >= g_klog_state.number_loggers_created) {
        kdprintf("Trying to set level for logger %d, when only %d loggers exist\n", p_logger_handle->value, g_klog_state.number_loggers_created);
        exit(1);
    }

    g_klog_state.a_logger_levels[p_logger_handle->value] = updated_level;
#endif
}

void klog_log(const KlogLoggerHandle* const p_logger_handle, const enum KlogLevel requested_level, const char* const s_filename, const uint32_t line_number, const char* const s_format, ...) {
#ifdef KLOG_OFF
    (void)p_logger_handle;
    (void)requested_level;
    (void)file;
    (void)line_number;
    (void)format;
#else
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (p_logger_handle->value >= g_klog_state.number_loggers_created) {
        kdprintf("Trying to log with logger %d, when only %d loggers exist\n", p_logger_handle->value, g_klog_state.number_loggers_created);
        exit(1);
    }

    if (requested_level > g_klog_state.a_logger_levels[p_logger_handle->value]) {
        return;
    }

    /* We are getting the time first, so it's closest to the actual point of invocation */
    const KlogString packed_time = g_klog_config.format.use_timestamp ? klog_format_time() : (KlogString){0, NULL};
    const KlogString* const p_packed_time = g_klog_config.format.use_timestamp ? &packed_time : NULL;

    /* Create the input string with the arguments injected */
    va_list p_args;
    va_start(p_args, s_format);
    const char* const s_input_message = klog_format_input_message(s_format, p_args);
    va_end(p_args);

    /* Split the input string into multiple input strings based on the newlines */
    const KlogFormatSplitInfo split_messages_info = klog_format_split_strings(s_input_message);

    /* Get the information to create the message header */
    const uint32_t thread_id = (uint32_t)klog_platform_get_current_thread_id();
    const char* const s_logger_name = &(g_klog_state.b_logger_names[p_logger_handle->value * g_klog_config.format.logger_name_max_length]);
    const char* const s_level = &(g_klog_state.b_level_strings[G_klog_level_string_length * requested_level]);
    const char* const s_level_colored = &(g_klog_state.b_level_strings_colored[G_klog_colored_level_string_length * requested_level]);

    const uint32_t* const p_thread_id = g_klog_config.format.use_thread_id ? &thread_id : NULL;
    const KlogString packed_name = {g_klog_config.format.logger_name_max_length, s_logger_name};
    const KlogString packed_level_color = {G_klog_colored_level_string_length, s_level_colored};
    const KlogString packed_level_file = {G_klog_level_string_length, s_level};
    const KlogString* const p_packed_level_stdout = g_klog_config.console.use_color ? &packed_level_color : &packed_level_file;

    const KlogString packed_source_location = (g_klog_config.format.source_location_filename_max_length && s_filename) ?
        klog_format_source_location(g_klog_config.format.source_location_filename_max_length, s_filename, line_number) :
        (KlogString){0, NULL};
    const KlogString* const p_packed_source_location = (g_klog_config.format.source_location_filename_max_length && s_filename) ? & packed_source_location : NULL;

    for (uint32_t i_message = 0; i_message < split_messages_info.number_strings; ++i_message) {
        const KlogString packed_message = {split_messages_info.a_string_lengths[i_message], split_messages_info.ls_strings[i_message]};

        if (requested_level <= g_klog_config.console.max_level) {
            klog_output_console(p_thread_id, p_packed_time, &packed_name, p_packed_level_stdout, p_packed_source_location, &packed_message);
        }
        if (g_klog_state.p_file && requested_level <= g_klog_config.file.max_level) {
            klog_output_file(g_klog_state.p_file, p_thread_id, p_packed_time, &packed_name, &packed_level_file, p_packed_source_location, &packed_message);
        }
    }

    free((char*)s_input_message);

    free(split_messages_info.ls_strings);
    free((uint32_t*)split_messages_info.a_string_lengths);

    if (g_klog_config.format.use_timestamp) {
        free((char*)packed_time.s);
    }

    if (g_klog_config.format.source_location_filename_max_length) {
        free((char*)packed_source_location.s);
    }
#endif
}
