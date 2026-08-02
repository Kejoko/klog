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
    klog_platform_semaphore_wait(g_klog_state.p_semaphore_messages_full);

    klog_platform_mutex_lock(g_klog_state.p_mutex_deinitialize);
    const bool should_deinit = !g_klog_state.is_initialized;
    klog_platform_mutex_unlock(g_klog_state.p_mutex_deinitialize);

    klog_platform_mutex_lock(g_klog_state.p_mutex_shared);
    /**
     * The variables in here which are actually shared amongst producer and consumer:
     *  - message_produced_total_count
     *  - message_unconsumed_count
     */
    if (g_klog_state.message_produced_total_count == g_klog_state.message_consumed_total_count) {
        /* We have logged everything we should have - up to this point. Should we stop?? */
        if (should_deinit) {
            /* We should stop */
            klog_platform_mutex_unlock(g_klog_state.p_mutex_shared);
            return true;
        }
    }
    if (g_klog_state.message_unconsumed_count < 1) {
        kdprintf("klog_async_consume consumed too many messages\n");
        exit(1);
    }
    g_klog_state.message_consumed_total_count++;
    g_klog_state.message_unconsumed_count = g_klog_state.message_unconsumed_count - 1;
    klog_platform_mutex_unlock(g_klog_state.p_mutex_shared);

    klog_platform_mutex_lock(g_klog_state.p_mutex_consumer);
    /* @todo This can move out of the shared block behind a consumer specific lock */
    /* Get our current consuming index and update the next consumer's index into the ring buffers */
    const uint32_t message_element_consumer_idx = g_klog_state.message_element_consumer_idx;
    g_klog_state.message_element_consumer_idx   = g_klog_state.message_element_consumer_idx + 1;
    if (g_klog_state.message_element_consumer_idx >= g_klog_state.message_element_count) {
        g_klog_state.message_element_consumer_idx = 0;
    }
    klog_platform_mutex_unlock(g_klog_state.p_mutex_consumer);

    /* Copy everything from the producer's buffers to our staging buffers, let the producer know it can go again */
    const uint32_t offset_file               = (g_klog_state.prefix_file_size + 1) * message_element_consumer_idx;
    const uint32_t offset_console            = (g_klog_state.prefix_console_size + 1) * message_element_consumer_idx;
    const uint32_t offset_messages_formatted = g_klog_state.message_formatted_max_size * message_element_consumer_idx;
    enum KlogLevel requested_level           = 0;
    uint32_t       actual_message_length     = 0;
    {
        /* These operations are done in the same order as in the producer, so we can have a higher degree of parellelism */

        klog_platform_mutex_lock(g_klog_state.p_mutex_message_levels);
        requested_level = g_klog_state.b_message_levels[message_element_consumer_idx];
        klog_platform_mutex_unlock(g_klog_state.p_mutex_message_levels);

        klog_platform_mutex_lock(g_klog_state.p_mutex_messages_formatted);
        memcpy(
            g_klog_state.b_messages_formatted_staging + offset_messages_formatted,
            g_klog_state.b_messages_formatted + offset_messages_formatted,
            g_klog_state.message_formatted_max_size
        );
        klog_platform_mutex_unlock(g_klog_state.p_mutex_messages_formatted);

        klog_platform_mutex_lock(g_klog_state.p_mutex_message_lengths);
        actual_message_length = g_klog_state.b_message_lengths[message_element_consumer_idx];
        klog_platform_mutex_unlock(g_klog_state.p_mutex_message_lengths);

        klog_platform_mutex_lock(g_klog_state.p_mutex_prefixes_file);
        memcpy(
            g_klog_state.b_prefixes_file_staging + offset_file,
            g_klog_state.b_prefixes_file + offset_file,
            g_klog_state.prefix_file_size + 1
        );
        klog_platform_mutex_unlock(g_klog_state.p_mutex_prefixes_file);

        klog_platform_mutex_lock(g_klog_state.p_mutex_prefixes_console);
        memcpy(
            g_klog_state.b_prefixes_console_staging + offset_console,
            g_klog_state.b_prefixes_console + offset_console,
            g_klog_state.prefix_console_size + 1
        );
        klog_platform_mutex_unlock(g_klog_state.p_mutex_prefixes_console);

        klog_platform_semaphore_signal(g_klog_state.p_semaphore_messages_empty);
    }

    /* Get the formatted message from the message buffer */
    char* s_message_formatted = g_klog_state.b_messages_formatted_staging + offset_messages_formatted;

    /* Get the console and file prefixes */
    char*            s_prefix_file         = g_klog_state.b_prefixes_file_staging + offset_file;
    char*            s_prefix_console      = g_klog_state.b_prefixes_console_staging + offset_console;
    const KlogString packed_prefix_file    = { g_klog_state.prefix_file_size, s_prefix_file };
    const KlogString packed_prefix_console = { g_klog_state.prefix_console_size, s_prefix_console };

    klog_output(
        actual_message_length,
        s_message_formatted,
        packed_prefix_console,
        packed_prefix_file,
        requested_level,
        g_klog_config.console.max_level,
        g_klog_config.file.max_level,
        g_klog_state.p_file,
        g_klog_state.p_mutex_output_console,
        g_klog_state.p_mutex_output_file
    );

    return false;
}
