#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./klog_state.h"
#include "./klog_async.h"
#include "./klog_constants.h"
#include "./klog_handle.h"
#include "./klog_platform.h"
#include "./klog_debug_util.h"
#include "./klog_initialize.h"
#include "./klog_output.h"
#include "./klog_format.h"

void klog_initialize(
    const uint32_t               max_number_loggers,
    const KlogFormatInfo         klog_format_info,
    const KlogAsyncInfo* const   p_klog_async_info,
    const KlogConsoleInfo* const p_klog_console_info,
    const KlogFileInfo* const    p_klog_file_info,
    const KlogAllocInfo* const   p_klog_alloc_info
) {
#ifdef KLOG_OFF
    (void)max_number_loggers;
    (void)klog_format_info;
    (void)p_klog_async_info;
    (void)p_klog_console_info;
    (void)p_klog_file_info;
    (void)p_klog_alloc_info;
#else
    if (
        !klog_initialize_are_parameters_valid(
            g_klog_state.is_initialized,
            max_number_loggers,
            klog_format_info,
            p_klog_async_info,
            p_klog_console_info,
            p_klog_file_info,
            p_klog_alloc_info
        )
    ) {
        exit(KLOG_EXIT_CODE);
    }

    g_klog_state.is_initialized = true; /* Set this upfront (before threads) so they don't hang while waiting for it */

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
    if (p_klog_alloc_info) {
        g_klog_config.alloc = *p_klog_alloc_info;
    } else {
        g_klog_config.alloc.alloc_cb = &malloc;
        g_klog_config.alloc.free_cb  = &free;
    }

    g_klog_state.number_loggers_max = max_number_loggers;

    g_klog_state.b_logger_names = klog_initialize_buffer(
        g_klog_state.number_loggers_max,
        g_klog_config.format.logger_name_max_length,
        ' ',
        false,
        g_klog_config.alloc.alloc_cb
    );
    kdprintf(
        "b_logger_names: %p through %p\n",
        (void*)g_klog_state.b_logger_names,
        (void*)(g_klog_state.b_logger_names + (max_number_loggers * klog_format_info.logger_name_max_length))
    );

    g_klog_state.a_logger_levels = klog_initialize_logger_levels_array(g_klog_state.number_loggers_max, g_klog_config.alloc.alloc_cb);
    kdprintf(
        "a_logger_levels: %p through %p\n",
        (void*)g_klog_state.a_logger_levels,
        (void*)(g_klog_state.a_logger_levels + g_klog_state.number_loggers_max)
    );

    g_klog_state.b_level_strings = klog_initialize_level_strings_buffer(g_klog_config.alloc.alloc_cb);
    kdprintf(
        "b_level_strings: %p through %p\n",
        (void*)g_klog_state.b_level_strings,
        (void*)(g_klog_state.b_level_strings + (G_klog_level_string_length * G_klog_number_levels))
    );

    g_klog_state.b_level_strings_colored = klog_initialize_colored_level_strings_buffer(g_klog_config.alloc.alloc_cb);
    kdprintf(
        "b_level_strings_colored: %p through %p\n",
        (void*)g_klog_state.b_level_strings_colored,
        (void*)(g_klog_state.b_level_strings_colored + (G_klog_colored_level_string_length * G_klog_number_levels))
    );

    g_klog_state.number_loggers_created = 0;

    g_klog_state.a_logger_handles = klog_initialize_logger_handle_array(g_klog_state.number_loggers_max, g_klog_config.alloc.alloc_cb);
    kdprintf(
        "a_logger_handles: %p through %p\n",
        (void*)g_klog_state.a_logger_handles,
        (void*)(g_klog_state.a_logger_handles + (g_klog_state.number_loggers_max * sizeof(KlogLoggerHandle)))
    );

    g_klog_state.prefix_file_size = klog_format_prefix_length_get(
        g_klog_config.format.use_thread_id,
        g_klog_config.format.use_timestamp,
        g_klog_config.format.logger_name_max_length,
        false,
        g_klog_config.format.source_location_filename_max_length
    );
    g_klog_state.prefix_console_size = klog_format_prefix_length_get(
        g_klog_config.format.use_thread_id,
        g_klog_config.format.use_timestamp,
        g_klog_config.format.logger_name_max_length,
        g_klog_config.console.use_color,
        g_klog_config.format.source_location_filename_max_length
    );
    g_klog_state.prefix_time_size             = G_klog_time_string_length;
    g_klog_state.prefix_source_location_size  = g_klog_config.format.source_location_filename_max_length + 4 + 1; /* 4 digit line number, colon */
    g_klog_state.message_unconsumed_count     = 0;
    g_klog_state.message_element_producer_idx = 0;
    g_klog_state.message_element_consumer_idx = 0;
    g_klog_state.message_produced_total_count = 0;
    g_klog_state.message_consumed_total_count = 0;
    if (p_klog_async_info) {
        g_klog_state.message_element_count = p_klog_async_info->message_queue_number_elements;
    } else {
        g_klog_state.message_element_count = 1;
    }
    g_klog_state.b_prefixes_file = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_file_size,
        '\0',
        true,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_prefixes_file_staging = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_file_size,
        '\0',
        true,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_prefixes_console = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_console_size,
        '\0',
        true,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_prefixes_console_staging = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_console_size,
        '\0',
        true,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_prefixes_time = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_time_size,
        '$',
        true,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_prefixes_source_location = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.prefix_source_location_size,
        '@',
        true,
        g_klog_config.alloc.alloc_cb
    );

    g_klog_state.message_formatted_max_size = g_klog_config.format.message_max_length + 1; /* Account for null termination */
    g_klog_state.b_messages_formatted       = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.message_formatted_max_size, /* Each message slot is null terminated */
        '\0',
        false,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_messages_formatted_staging = klog_initialize_buffer(
        g_klog_state.message_element_count,
        g_klog_state.message_formatted_max_size, /* Each message slot is null terminated */
        '\0',
        false,
        g_klog_config.alloc.alloc_cb
    );
    g_klog_state.b_message_lengths = g_klog_config.alloc.alloc_cb(
        sizeof(*g_klog_state.b_message_lengths)
        * g_klog_state.message_element_count
    );
    memset(
        g_klog_state.b_message_lengths,
        0,
        sizeof(*g_klog_state.b_message_lengths)
        * g_klog_state.message_element_count
    );

    g_klog_state.b_message_levels = g_klog_config.alloc.alloc_cb(
        sizeof(*g_klog_state.b_message_levels)
        * g_klog_state.message_element_count
    );
    memset(
        g_klog_state.b_message_levels,
        0,
        sizeof(*g_klog_state.b_message_levels)
        * g_klog_state.message_element_count
    );

    g_klog_state.s_filename = klog_format_filename(p_klog_file_info, g_klog_config.alloc.alloc_cb, g_klog_config.alloc.free_cb);
    g_klog_state.p_file     = klog_initialize_file(g_klog_state.s_filename);

    g_klog_state.p_mutex_deinitialize        = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_logger_modification = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_message_levels      = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_messages_formatted  = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_message_lengths     = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_prefixes_file       = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_prefixes_console    = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_shared              = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_producer            = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_consumer            = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_output_console      = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    g_klog_state.p_mutex_output_file         = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_mutex_t));
    klog_platform_mutex_initialize(g_klog_state.p_mutex_deinitialize);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_logger_modification);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_message_levels);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_messages_formatted);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_message_lengths);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_prefixes_file);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_prefixes_console);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_shared);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_producer);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_consumer);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_output_console);
    klog_platform_mutex_initialize(g_klog_state.p_mutex_output_file);

    g_klog_state.p_semaphore_messages_empty = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_semaphore_t));
    g_klog_state.p_semaphore_messages_full  = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_semaphore_t));
    klog_platform_semaphore_initialize(g_klog_state.p_semaphore_messages_empty, g_klog_state.message_element_count);
    klog_platform_semaphore_initialize(g_klog_state.p_semaphore_messages_full,  0);

    if (g_klog_config.async.number_backing_threads > 0) {
        g_klog_state.b_threads = g_klog_config.alloc.alloc_cb(sizeof(klog_platform_thread_t) * g_klog_config.async.number_backing_threads);
        for (uint32_t idx_thread = 0; idx_thread < g_klog_config.async.number_backing_threads; ++idx_thread) {
            klog_platform_thread_create(&g_klog_state.b_threads[idx_thread], klog_async_thread_body, NULL);
        }
    }
#endif
}

void klog_deinitialize(
    void
) {
#ifdef KLOG_OFF
#else
    klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(KLOG_EXIT_CODE);
    }

    g_klog_state.is_initialized = false;

    /* Ensure each of the consumers goes through the semaphore wait, into the check */
    for (uint32_t idx_consumer = 0; idx_consumer < g_klog_config.async.number_backing_threads; ++idx_consumer) {
        klog_platform_semaphore_signal(g_klog_state.p_semaphore_messages_full);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);

    if (g_klog_state.b_threads) {
        for (uint32_t idx_thread = 0; idx_thread < g_klog_config.async.number_backing_threads; ++idx_thread) {
            klog_platform_thread_join(&g_klog_state.b_threads[idx_thread], NULL);
        }
        g_klog_config.alloc.free_cb(g_klog_state.b_threads);
    }
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_deinitialize);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_logger_modification);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_message_levels);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_messages_formatted);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_message_lengths);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_prefixes_file);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_prefixes_console);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_shared);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_producer);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_consumer);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_output_console);
    klog_platform_mutex_deinitialize(g_klog_state.p_mutex_output_file);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_deinitialize);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_logger_modification);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_message_levels);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_messages_formatted);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_message_lengths);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_prefixes_file);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_prefixes_console);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_shared);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_producer);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_consumer);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_output_console);
    g_klog_config.alloc.free_cb(g_klog_state.p_mutex_output_file);
    g_klog_state.b_threads                   = NULL;
    g_klog_state.p_mutex_deinitialize        = NULL;
    g_klog_state.p_mutex_logger_modification = NULL;
    g_klog_state.p_mutex_message_levels      = NULL;
    g_klog_state.p_mutex_messages_formatted  = NULL;
    g_klog_state.p_mutex_message_lengths     = NULL;
    g_klog_state.p_mutex_prefixes_file       = NULL;
    g_klog_state.p_mutex_prefixes_console    = NULL;
    g_klog_state.p_mutex_shared              = NULL;
    g_klog_state.p_mutex_producer            = NULL;
    g_klog_state.p_mutex_consumer            = NULL;
    g_klog_state.p_mutex_output_console      = NULL;
    g_klog_state.p_mutex_output_file         = NULL;

    klog_platform_semaphore_deinitialize(g_klog_state.p_semaphore_messages_empty);
    klog_platform_semaphore_deinitialize(g_klog_state.p_semaphore_messages_full);
    g_klog_config.alloc.free_cb(g_klog_state.p_semaphore_messages_empty);
    g_klog_config.alloc.free_cb(g_klog_state.p_semaphore_messages_full);
    g_klog_state.p_semaphore_messages_empty = NULL;
    g_klog_state.p_semaphore_messages_full  = NULL;

    g_klog_state.number_loggers_max     = 0;
    g_klog_state.number_loggers_created = 0;

    g_klog_config.alloc.free_cb(g_klog_state.a_logger_handles);
    g_klog_config.alloc.free_cb(g_klog_state.b_logger_names);
    g_klog_config.alloc.free_cb(g_klog_state.a_logger_levels);
    g_klog_state.a_logger_handles = NULL;
    g_klog_state.b_logger_names   = NULL;
    g_klog_state.a_logger_levels  = NULL;

    g_klog_config.alloc.free_cb(g_klog_state.b_level_strings);
    g_klog_config.alloc.free_cb(g_klog_state.b_level_strings_colored);
    g_klog_state.b_level_strings         = NULL;
    g_klog_state.b_level_strings_colored = NULL;

    g_klog_state.message_unconsumed_count     = 0;
    g_klog_state.message_element_producer_idx = 0;
    g_klog_state.message_element_consumer_idx = 0;
    g_klog_state.message_element_count        = 0;
    g_klog_state.message_produced_total_count = 0;
    g_klog_state.message_consumed_total_count = 0;

    g_klog_state.prefix_file_size = 0;
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_file);
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_file_staging);
    g_klog_state.b_prefixes_file         = NULL;
    g_klog_state.b_prefixes_file_staging = NULL;

    g_klog_state.prefix_console_size = 0;
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_console);
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_console_staging);
    g_klog_state.b_prefixes_console         = NULL;
    g_klog_state.b_prefixes_console_staging = NULL;

    g_klog_state.prefix_time_size = 0;
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_time);
    g_klog_state.b_prefixes_time = NULL;

    g_klog_state.prefix_source_location_size = 0;
    g_klog_config.alloc.free_cb(g_klog_state.b_prefixes_source_location);
    g_klog_state.b_prefixes_source_location = NULL;

    g_klog_state.message_formatted_max_size = 0;
    g_klog_config.alloc.free_cb(g_klog_state.b_messages_formatted);
    g_klog_config.alloc.free_cb(g_klog_state.b_messages_formatted_staging);
    g_klog_state.b_messages_formatted         = NULL;
    g_klog_state.b_messages_formatted_staging = NULL;
    g_klog_config.alloc.free_cb(g_klog_state.b_message_lengths);
    g_klog_state.b_message_lengths = NULL;

    g_klog_config.alloc.free_cb(g_klog_state.b_message_levels);
    g_klog_state.b_message_levels = NULL;

    if (g_klog_state.s_filename) {
        g_klog_config.alloc.free_cb(g_klog_state.s_filename);
        g_klog_state.s_filename = NULL;
    }
    if (g_klog_state.p_file) {
        fclose(g_klog_state.p_file);
        g_klog_state.p_file = NULL;
    }

    /* Need to do this near the end because we need the callbacks for freeing */
    g_klog_config = (struct KlogConfig) { 0 };
#endif
}

const KlogLoggerHandle* klog_logger_create(
    const char* const s_logger_name,
    const uint32_t    name_length
) {
#ifdef KLOG_OFF
    (void)logger_name;
    return NULL;
#else
    klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(KLOG_EXIT_CODE);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);

    klog_platform_mutex_lock(g_klog_state.p_mutex_logger_modification);

    if (g_klog_state.number_loggers_created >= g_klog_state.number_loggers_max) {
        kdprintf("Trying to create klog logger, but klog only allows %d loggers\n", g_klog_state.number_loggers_max);
        exit(KLOG_EXIT_CODE);
    }

    if (name_length <= 0) {
        kdprintf("Trying to create klog logger, the length of the provided name is 0\n");
        exit(KLOG_EXIT_CODE);
    }

    const uint32_t current_logger_index = g_klog_state.number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_index * g_klog_config.format.logger_name_max_length;
    klog_format_logger_name(
        s_logger_name,
        name_length,
        &g_klog_state.b_logger_names[logger_name_start_index], /* This is initialized to spaces */
        g_klog_config.format.logger_name_max_length
    );

    g_klog_state.a_logger_levels[current_logger_index] = KLOG_LEVEL_OFF;

    /* We have to do this weird casting stuff to prevent valgrind from exploding */
    KlogLoggerHandle* const p_logger_handle = &(((KlogLoggerHandle*)g_klog_state.a_logger_handles)[current_logger_index]);
    p_logger_handle->value                  = current_logger_index;

    g_klog_state.number_loggers_created++;

    klog_platform_mutex_unlock(g_klog_state.p_mutex_logger_modification);

    return p_logger_handle;
#endif
}

void klog_logger_level_set(
    const KlogLoggerHandle* const p_logger_handle,
    const enum KlogLevel          updated_level
) {
#ifdef KLOG_OFF
    (void)p_logger_handle;
    (void)updated_level;
#else
    klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(KLOG_EXIT_CODE);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);

    klog_platform_mutex_lock(g_klog_state.p_mutex_logger_modification);

    if (p_logger_handle->value >= g_klog_state.number_loggers_created) {
        kdprintf(
            "Trying to set level for logger %d, when only %d loggers exist\n",
            p_logger_handle->value,
            g_klog_state.number_loggers_created
        );
        exit(KLOG_EXIT_CODE);
    }

    if (updated_level > KLOG_LEVEL_TRACE) {
        kdprintf("Trying to set level (%d) greater than trace (%d)\n", updated_level, KLOG_LEVEL_TRACE);
        exit(KLOG_EXIT_CODE);
    }

    g_klog_state.a_logger_levels[p_logger_handle->value] = updated_level;

    klog_platform_mutex_unlock(g_klog_state.p_mutex_logger_modification);
#endif
}

void klog_log(
    const KlogLoggerHandle* const p_logger_handle,
    const enum KlogLevel          requested_level,
    const char* const             s_filename,
    const uint32_t                line_number,
    const char* const             s_format,
    ...
) {
#ifdef KLOG_OFF
    (void)p_logger_handle;
    (void)requested_level;
    (void)file;
    (void)line_number;
    (void)format;
#else
    klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);
    if (!g_klog_state.is_initialized) {
        kdprintf("Trying to create klog logger, but klog is not initialized\n");
        exit(KLOG_EXIT_CODE);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);

    klog_platform_mutex_lock(g_klog_state.p_mutex_logger_modification);
    if (p_logger_handle->value >= g_klog_state.number_loggers_created) {
        kdprintf("Trying to log with logger %d, when only %d loggers exist\n", p_logger_handle->value, g_klog_state.number_loggers_created);
        exit(KLOG_EXIT_CODE);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_logger_modification);

    if (requested_level > KLOG_LEVEL_TRACE) {
        kdprintf("Trying to log with level (%d) greater than trace (%d)\n", requested_level, KLOG_LEVEL_TRACE);
        exit(KLOG_EXIT_CODE);
    }

    if (requested_level == 0) {
        kdprintf("Trying to log with the level set to OFF\n");
        return;
    }

    if ((requested_level > g_klog_config.console.max_level) && (requested_level > g_klog_config.file.max_level)) {
        kdprintf("Trying to log with a level that neither console nor file accept\n");
        return;
    }

    klog_platform_mutex_lock(g_klog_state.p_mutex_logger_modification);
    if (requested_level > g_klog_state.a_logger_levels[p_logger_handle->value]) {
        kdprintf("Trying to log with a level more verbose than the requested logger allows\n");
        klog_platform_mutex_unlock(g_klog_state.p_mutex_logger_modification);
        return;
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_logger_modification);

    klog_platform_semaphore_wait(g_klog_state.p_semaphore_messages_empty);

    klog_platform_mutex_lock(g_klog_state.p_mutex_shared);
    g_klog_state.message_produced_total_count++;
    g_klog_state.message_unconsumed_count = g_klog_state.message_unconsumed_count + 1;
    if (g_klog_state.message_unconsumed_count > g_klog_state.message_element_count) {
        kdprintf(
            "klog_log produced too many messages (%d unconsumed messages when we only have space for %d)\n",
            g_klog_state.message_unconsumed_count,
            g_klog_state.message_element_count
        );
        exit(1);
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_shared);

    klog_platform_mutex_lock(g_klog_state.p_mutex_producer);
    /* Update the producer's index into our ring buffer */
    const uint32_t message_element_producer_idx = g_klog_state.message_element_producer_idx;
    g_klog_state.message_element_producer_idx   = g_klog_state.message_element_producer_idx + 1;
    if (g_klog_state.message_element_producer_idx >= g_klog_state.message_element_count) {
        g_klog_state.message_element_producer_idx = 0;
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_producer);

    /**
     * @brief We don't need to put a lock around the b_prefixes_time here, because it is gauranteed that only one
     *      producer can be here with this specific message_element_producer_idx at a time. So no two producers can possibly
     *      be modifying the same part of this buffer, and the consumer doesn't look in here, so we are safe.
     */
    /* We are getting the time first, so it's closest to the actual point of invocation */
    char* s_prefix_time = g_klog_state.b_prefixes_time + (message_element_producer_idx * g_klog_state.prefix_time_size);
    memset(s_prefix_time, '\0', g_klog_state.prefix_time_size);
    const KlogString packed_time = g_klog_config.format.use_timestamp ? klog_format_time(s_prefix_time) : (KlogString) { 0, NULL };

    /* Get the information to create the message prefixes */
    /* b_logger_names doesn't need to be behind a mutex because it will never get updated after creation */
    const uint32_t    thread_id         = (uint32_t)klog_platform_get_current_thread_id();
    const uint32_t    logger_name_index = p_logger_handle->value * g_klog_config.format.logger_name_max_length;
    const char* const s_logger_name     = &(g_klog_state.b_logger_names[logger_name_index]);
    const char* const s_level           = &(g_klog_state.b_level_strings[G_klog_level_string_length * requested_level]);
    const char* const s_level_colored   = &(g_klog_state.b_level_strings_colored[G_klog_colored_level_string_length * requested_level]);

    /* More message prefixe prep */
    const uint32_t* const   p_thread_id            = g_klog_config.format.use_thread_id ? &thread_id : NULL;
    const KlogString        packed_name            = { g_klog_config.format.logger_name_max_length, s_logger_name };
    const KlogString        packed_level_color     = { G_klog_colored_level_string_length, s_level_colored };
    const KlogString        packed_level_file      = { G_klog_level_string_length, s_level };
    const KlogString* const p_packed_level_console = g_klog_config.console.use_color ? &packed_level_color : &packed_level_file;

    /**
     * @brief We don't need to put a lock around the b_prefixes_source_location here, because it is gauranteed that only one
     *      producer can be here with this specific message_element_producer_idx at a time. So no two producers can possibly
     *      be modifying the same part of this buffer, and the consumer doesn't look in here, so we are safe.
     */
    /* Source location for the message prefix */
    char* s_prefix_source_location = g_klog_state.b_prefixes_source_location
        + (message_element_producer_idx * g_klog_state.prefix_source_location_size);
    const KlogString packed_source_location = (g_klog_config.format.source_location_filename_max_length && s_filename)
        ? klog_format_source_location(
                s_prefix_source_location,
                g_klog_config.format.source_location_filename_max_length,
                s_filename,
                line_number
            )
        : (KlogString) { 0, NULL };

    klog_platform_mutex_lock(g_klog_state.p_mutex_message_levels);
    /* Update the level buffer */
    g_klog_state.b_message_levels[message_element_producer_idx] = requested_level;
    klog_platform_mutex_unlock(g_klog_state.p_mutex_message_levels);

    klog_platform_mutex_lock(g_klog_state.p_mutex_messages_formatted);
    /* Create the input string with the arguments injected - including space for null termination */
    char* s_message_formatted = g_klog_state.b_messages_formatted
        + (message_element_producer_idx * g_klog_state.message_formatted_max_size);
    /* Clear the message buffer for the formatted message we just used */
    memset(s_message_formatted, 0, g_klog_state.message_formatted_max_size);
    va_list p_args;
    va_start(p_args, s_format);
    const uint32_t actual_message_length = klog_format_input_message(
        s_message_formatted,
        g_klog_state.message_formatted_max_size,
        s_format,
        p_args
    );
    va_end(p_args);
    klog_platform_mutex_unlock(g_klog_state.p_mutex_messages_formatted);

    klog_platform_mutex_lock(g_klog_state.p_mutex_message_lengths);
    /* Update the length buffer */
    g_klog_state.b_message_lengths[message_element_producer_idx] = actual_message_length;
    klog_platform_mutex_unlock(g_klog_state.p_mutex_message_lengths);

    klog_platform_mutex_lock(g_klog_state.p_mutex_prefixes_file);
    /* Actually create the file prefix */
    char* s_prefix_file = g_klog_state.b_prefixes_file
        + (message_element_producer_idx * (g_klog_state.prefix_file_size + 1));
    memset(s_prefix_file, '\0', g_klog_state.prefix_file_size);
    const KlogString packed_prefix_file = klog_format_message_prefix(
        s_prefix_file,
        p_thread_id,
        &packed_time,
        &packed_name,
        &packed_level_file,
        &packed_source_location
    );
    klog_platform_mutex_unlock(g_klog_state.p_mutex_prefixes_file);

    klog_platform_mutex_lock(g_klog_state.p_mutex_prefixes_console);
    /* Actually create the console prefix */
    char* s_prefix_console = g_klog_state.b_prefixes_console
        + (message_element_producer_idx * (g_klog_state.prefix_console_size + 1));
    memset(s_prefix_console, '\0', g_klog_state.prefix_console_size);
    const KlogString packed_prefix_console = klog_format_message_prefix(
        s_prefix_console,
        p_thread_id,
        &packed_time,
        &packed_name,
        p_packed_level_console,
        &packed_source_location
    );
    klog_platform_mutex_unlock(g_klog_state.p_mutex_prefixes_console);

    klog_platform_semaphore_signal(g_klog_state.p_semaphore_messages_full);

    (void)packed_prefix_file;
    (void)packed_prefix_console;

    /* If we have consumer threads, we're done here. Else, consume what we just made */
    if (g_klog_config.async.number_backing_threads > 0) {
        return;
    }

    /**
     * @brief We are still leveraging the consumer's function here because we need to guard against
     *      the case where this klog_log function is being invoked from multiple threads without a
     *      backing consumer. This will prevent any weirdness from happening if two threads are
     *      both trying to produce logs at the same time
     */
    klog_async_consume();
#endif
}
