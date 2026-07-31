#include "./klog_async.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

/**
 * @brief This is the ONLY non klog.c source file which is allowed to directly include and
 *      use the klog_state.h header file. This is because we use so much of the state in these
 *      async functions, that it would be incredibly cumbersome to pass everything in via
 *      parameters. Maybe we ought to revisit this decision in the future.
 */
#include "./klog_state.h"
#include "./klog_debug_util.h"
#include "./klog_format.h"
#include "./klog_output.h"
#include "./klog_platform.h"

void* klog_async_thread_body(
    void* p
) {
    (void)p;

    while (true) {
        bool should_stop = klog_async_consume();
        if (should_stop) {
            break;
        }
    }

    return NULL;
}

bool klog_async_consume(
    void
) {
    klog_platform_mutex_lock(g_klog_state.p_mutex_consumer);

    klog_platform_semaphore_wait(g_klog_state.p_semaphore_messages_full);

    klog_platform_mutex_lock(g_klog_state.p_mutex_shared);

    if (g_klog_state.message_produced_total_count == g_klog_state.message_consumed_total_count) {
        /* We have logged everything we should have - up to this point. Should we stop?? */
        klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);

        if (!g_klog_state.is_initialized) {
            /* We should stop */
            klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);
            klog_platform_mutex_unlock(g_klog_state.p_mutex_shared);
            klog_platform_mutex_unlock(g_klog_state.p_mutex_consumer);
            return true;
        }

        /* Let's keep going */
        klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);
    }

    /* Get the level from the global buffer of levels corresponding to messages */
    const enum KlogLevel requested_level = g_klog_state.b_message_levels[g_klog_state.message_element_consumer_idx];

    /* Get the formatted message from the message buffer */
    char* s_message_formatted = g_klog_state.b_messages_formatted
        + (g_klog_state.message_element_consumer_idx * g_klog_state.message_formatted_max_size);

    const uint32_t actual_message_length = g_klog_state.b_message_lengths[g_klog_state.message_element_consumer_idx];

    /* Get the console and file prefixes */
    char* s_prefix_file    = g_klog_state.b_prefixes_file + (g_klog_state.message_element_consumer_idx * g_klog_state.prefix_file_size);
    char* s_prefix_console = g_klog_state.b_prefixes_console
        + (g_klog_state.message_element_consumer_idx * g_klog_state.prefix_console_size);
    const KlogString packed_prefix_file    = { g_klog_state.prefix_file_size, s_prefix_file };
    const KlogString packed_prefix_console = { g_klog_state.prefix_console_size, s_prefix_console };

    uint32_t i_starting_character = 0;
    while (i_starting_character <= actual_message_length) {
        const char* const p_newline         = strchr(s_message_formatted + i_starting_character, '\n');
        const uint32_t    submessage_length = p_newline
            ? p_newline - (s_message_formatted + i_starting_character)
            : actual_message_length;

        const KlogString packed_message = { submessage_length, s_message_formatted + i_starting_character };
        if (requested_level <= g_klog_config.console.max_level) {
            klog_output_console(&packed_prefix_console, &packed_message);
        }
        if (g_klog_state.p_file && (requested_level <= g_klog_config.file.max_level)) {
            klog_output_file(g_klog_state.p_file, &packed_prefix_file, &packed_message);
        }

        i_starting_character = i_starting_character + submessage_length + 1;
    }

    /* Update the consumer's index into our ring buffer */
    g_klog_state.message_element_consumer_idx = g_klog_state.message_element_consumer_idx + 1;
    if (g_klog_state.message_element_consumer_idx >= g_klog_state.message_element_count) {
        g_klog_state.message_element_consumer_idx = 0;
    }

    if (g_klog_state.message_unconsumed_count < 1) {
        kdprintf("klog_async_consume consumed too many messages\n");
        exit(1);
    }
    g_klog_state.message_consumed_total_count++;
    g_klog_state.message_unconsumed_count = g_klog_state.message_unconsumed_count - 1;

    klog_platform_mutex_unlock(g_klog_state.p_mutex_shared);

    klog_platform_semaphore_signal(g_klog_state.p_semaphore_messages_empty);

    klog_platform_mutex_unlock(g_klog_state.p_mutex_consumer);

    return false;
}
