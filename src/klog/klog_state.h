#ifndef KLOG_STATE_INCLUDED
#define KLOG_STATE_INCLUDED

/**
 * @brief The only implementation file which can include this is klog.c, though
 *      the unit test files may also include this file so they can inspect the
 *      state after certain operations to ensure correctness
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "klog/klog.h"

extern struct KlogConfig {
    KlogFormatInfo  format;
    KlogAsyncInfo   async;
    KlogConsoleInfo console;
    KlogFileInfo    file;
} g_klog_config;

extern struct KlogState {
    uint32_t                 number_loggers_max;
    uint32_t                 number_loggers_created;
    struct KlogLoggerHandle* a_logger_handles;
    char*                    b_logger_names;
    uint8_t*                 a_logger_levels;
    char*                    b_level_strings;
    char*                    b_level_strings_colored;
    char*                    b_message_queue;
    FILE*                    p_file;
    bool                     is_initialized;
} g_klog_state;

#endif /* KLOG_STATE_INCLUDED */
