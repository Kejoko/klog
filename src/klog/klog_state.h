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

/**
 * @brief This represents the initial configuraton as set by the user
 */
extern struct KlogConfig {
    KlogFormatInfo  format;
    KlogAsyncInfo   async;
    KlogConsoleInfo console;
    KlogFileInfo    file;
} g_klog_config;

/**
 * @brief This represents the actual state of klog
 */
extern struct KlogState {
    uint32_t                 number_loggers_max;
    uint32_t                 number_loggers_created;

    struct KlogLoggerHandle* a_logger_handles;
    char*                    b_logger_names;
    uint8_t*                 a_logger_levels;

    char*                    b_level_strings;
    char*                    b_level_strings_colored;

    uint32_t                 prefix_element_index;
    uint32_t                 prefix_element_count;
    uint32_t                 prefix_file_size;
    char*                    b_prefixes_file;
    uint32_t                 prefix_console_size;
    char*                    b_prefixes_console;
    uint32_t                 prefix_time_size;
    char*                    b_prefixes_time;
    uint32_t                 prefix_source_location_size;
    char*                    b_prefixes_source_location;

    uint32_t                 message_element_index;
    uint32_t                 message_element_count;
    char*                    b_messages;

    FILE*                    p_file;

    bool                     is_initialized;
} g_klog_state;

#endif /* KLOG_STATE_INCLUDED */
