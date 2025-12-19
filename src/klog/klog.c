#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Whether or not klog has been initialized */
bool g_klog_is_initialized = false;

/*  The maximum number of loggers allowed */
uint32_t g_klog_max_number_loggers = 0;

/* The maximum length allowed for a logger's name */
uint32_t g_klog_max_logger_name_length = 0;
uint32_t g_klog_null_terminated_logger_name_length = 0;

/* The current number of loggers which actually exist */
uint32_t g_klog_current_number_loggers_created = 0;

/* Array of logger names, each null terminated */
char* gp_klog_logger_names = NULL;

/* Array of uint8_t for the current allowed level for each logger */
uint8_t* gp_klog_logger_levels = NULL;

/* Array of char* representing each of the logging levels in a nicely printable format, each null terminated */
char* gp_klog_level_strings = NULL;

/* ================================================================================================================== */
/* Constants                                                                                                          */
/* ================================================================================================================== */

const uint32_t G_klog_level_string_length = 8;
const uint32_t G_klog_number_levels = KLOG_LEVEL_TRACE + 1;

/* ================================================================================================================== */
/* Helper functions                                                                                                   */
/* ================================================================================================================== */

const char* klog_impl_get_level_string(const enum klog_level_e requested_level) {
    const uint32_t i = G_klog_level_string_length * requested_level;
    return &gp_klog_level_strings[i];
}

/* ================================================================================================================== */
/* API Implementation                                                                                                 */
/* ================================================================================================================== */

void klog_initialize(const uint32_t max_number_loggers, const uint32_t max_logger_name_length) {
    if (g_klog_is_initialized) {
        printf("Trying to initialize klog, when it is already initialized\n");
        exit(1);
    }

    if (max_number_loggers == 0) {
        printf("Trying to initialize klog with maximum number of loggers set to 0");
        exit(1);
    }

    if (max_logger_name_length == 0) {
        printf("Trying to initialize klog with maximum length of logger names set to 0");
        exit(1);
    }

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_max_logger_name_length = max_logger_name_length;
    g_klog_null_terminated_logger_name_length = g_klog_max_logger_name_length + 1;

    /* +1 to allow for 32 characters and null termination */
    /* set all characters to space (' ') by default, so we can null terminate at a fixed length */
    const uint32_t total_logger_names_array_size = g_klog_null_terminated_logger_name_length * g_klog_max_number_loggers;
    gp_klog_logger_names = malloc(total_logger_names_array_size);
    memset(gp_klog_logger_names, ' ', total_logger_names_array_size);

    const uint32_t total_logger_levels_array_size = g_klog_max_number_loggers;
    gp_klog_logger_levels = calloc(total_logger_levels_array_size, sizeof(gp_klog_logger_levels));

    const uint32_t level_string_array_total_bytes = G_klog_level_string_length * G_klog_number_levels; 
    gp_klog_level_strings = malloc(level_string_array_total_bytes);
    const char off_string[]      = "off     ";
    const char critical_string[] = "critical";
    const char error_string[]    = "error   ";
    const char warning_string[]  = "warning ";
    const char info_string[]     = "info    ";
    const char debug_string[]    = "debug   ";
    const char trace_string[]    = "trace   ";
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_OFF      * G_klog_number_levels], off_string, sizeof(off_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_CRITICAL * G_klog_number_levels], critical_string, sizeof(critical_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_ERROR    * G_klog_number_levels], error_string, sizeof(error_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_WARNING  * G_klog_number_levels], warning_string, sizeof(warning_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_INFO     * G_klog_number_levels], info_string, sizeof(info_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_DEBUG    * G_klog_number_levels], debug_string, sizeof(debug_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_TRACE    * G_klog_number_levels], trace_string, sizeof(trace_string));

    g_klog_current_number_loggers_created = 0;

    g_klog_is_initialized = true;
}

void klog_deinitialize(void) {
    if (!g_klog_is_initialized) {
        printf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(1);
    }

    g_klog_max_number_loggers = 0;
    g_klog_max_logger_name_length = 0;
    g_klog_current_number_loggers_created = 0;
    free(gp_klog_logger_names);
    free(gp_klog_logger_levels);
    free(gp_klog_level_strings);
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

    const klog_logger_handle_t current_logger_handle = g_klog_current_number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_handle * g_klog_null_terminated_logger_name_length;
    const uint32_t num_chars_to_copy = strlen(logger_name) >= g_klog_max_logger_name_length ?
        g_klog_max_logger_name_length : /* copy as much as we can fit */
        strlen(logger_name);          /* copy it all */
    memcpy(&gp_klog_logger_names[logger_name_start_index], logger_name, num_chars_to_copy);

    const uint32_t null_terminator_index = logger_name_start_index + g_klog_max_logger_name_length;
    gp_klog_logger_names[null_terminator_index] = '\0';

    const uint32_t logger_level_index = current_logger_handle;
    gp_klog_logger_levels[logger_level_index] = KLOG_LEVEL_OFF;

    g_klog_current_number_loggers_created++;

    return current_logger_handle;
}

void klog_logger_set_level(const klog_logger_handle_t logger_handle, const enum klog_level_e updated_level) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle >= g_klog_current_number_loggers_created) {
        printf("Trying to set level for logger %d, when only %d loggers exist\n", logger_handle, g_klog_current_number_loggers_created);
        exit(1);
    }

    gp_klog_logger_levels[logger_handle] = updated_level;
}

void klog(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle >= g_klog_current_number_loggers_created) {
        printf("Trying to log with logger %d, when only %d loggers exist\n", logger_handle, g_klog_current_number_loggers_created);
        exit(1);
    }

    const uint32_t current_level = gp_klog_logger_levels[logger_handle];
    if (requested_level > current_level) {
        return;
    }

    va_list args;
    va_start(args, format);
    /* +1 for null termination */
    int total_length = vsnprintf(0, 0, format, args) + 1;
    va_end(args);
    char* resulting_string = malloc(total_length);
    va_start(args, format);
    vsnprintf(resulting_string, total_length, format, args);
    va_end(args);

    const char* logger_name = &gp_klog_logger_names[logger_handle * g_klog_null_terminated_logger_name_length];
    const char* level_string = klog_impl_get_level_string(requested_level);

    printf("[%s] [%.*s] %s\n", logger_name, G_klog_level_string_length, level_string, resulting_string);

    free(resulting_string);
}
