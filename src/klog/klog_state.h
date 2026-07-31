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
#include "./klog_platform.h"

/**
 * @brief This represents the initial configuraton as set by the user
 */
extern struct KlogConfig {
    KlogFormatInfo  format;
    KlogAsyncInfo   async;
    KlogConsoleInfo console;
    KlogFileInfo    file;
    KlogAllocInfo   alloc;
} g_klog_config;

/**
 * @brief This represents the actual state of klog
 */
extern struct KlogState {
    uint32_t number_loggers_max;
    uint32_t number_loggers_created;

    struct KlogLoggerHandle* a_logger_handles;
    char*                    b_logger_names;
    uint8_t*                 a_logger_levels;

    char* b_level_strings;
    char* b_level_strings_colored;

    uint32_t message_unconsumed_count;
    uint32_t message_element_producer_idx;
    uint32_t message_element_consumer_idx;
    uint32_t message_element_count;
    uint32_t message_produced_total_count;
    uint32_t message_consumed_total_count;

    uint32_t prefix_file_size;
    char*    b_prefixes_file; /* Need copy */
    char*    b_prefixes_file_staging;
    uint32_t prefix_console_size;
    char*    b_prefixes_console; /* Need copy */
    char*    b_prefixes_console_staging;
    uint32_t prefix_time_size;
    char*    b_prefixes_time;
    uint32_t prefix_source_location_size;
    char*    b_prefixes_source_location;

    uint32_t  message_formatted_max_size;
    char*     b_messages_formatted; /* Need copy */
    char*     b_messages_formatted_staging;
    uint32_t* b_message_lengths; /* Need copy */
    uint32_t* b_message_lengths_staging;

    uint32_t* b_message_levels; /* Need copy */
    uint32_t* b_message_levels_staging;

    char* s_filename;
    FILE* p_file;

    bool is_initialized;

    klog_platform_thread_t*    b_threads;
    klog_platform_mutex_t*     p_mutex_deinitialize;
    klog_platform_mutex_t*     p_mutex_shared;
    klog_platform_mutex_t*     p_mutex_producer;
    klog_platform_mutex_t*     p_mutex_consumer;
    klog_platform_semaphore_t* p_semaphore_messages_empty;
    klog_platform_semaphore_t* p_semaphore_messages_full;
} g_klog_state;

#endif /* KLOG_STATE_INCLUDED */
