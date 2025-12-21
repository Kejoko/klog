#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#error "Klog does not support Windows"
#endif

#ifdef __APPLE__
#error "Klog does not support MacOS"
#endif

#ifdef  __linux__
#include <unistd.h>
#include <sys/syscall.h>
#ifndef SYS_gettid
#error "SYS_gettid is unavailable on this system"
#endif
#endif

/* ================================================================================================================== */
/* Klog globals                                                                                                       */
/* ================================================================================================================== */

/* Set by the user -------------------------------------------------------------------------------------------------- */

/* Whether or not klog has been initialized */
bool g_klog_is_initialized = false;

/*  The maximum number of loggers allowed */
uint32_t g_klog_max_number_loggers = 0;

/* The maximum length allowed for a logger's name */
uint32_t g_klog_logger_name_max_length = 0;

/* Internal state --------------------------------------------------------------------------------------------------- */

/* The current number of loggers which actually exist */
uint32_t g_klog_current_number_loggers_created = 0;

/* Array of logger names, non-null terminated */
char* gp_klog_logger_names = NULL;

/* Array of uint8_t for the current allowed level for each logger */
uint8_t* gp_klog_logger_levels = NULL;

/* Array of the logging levels stringified in a nicely printable format, all fixed width. Non-null terminated */
char* gp_klog_level_strings = NULL;

/* ================================================================================================================== */
/* Constants                                                                                                          */
/* ================================================================================================================== */

const uint32_t G_klog_level_string_length = 8;
const uint32_t G_klog_number_levels = KLOG_LEVEL_TRACE + 1;

/* ================================================================================================================== */
/* Helper functions                                                                                                   */
/* ================================================================================================================== */

bool klog_initialization_parameters_is_valid(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_size, const uint32_t message_max_length) {
    if (g_klog_is_initialized) {
        printf("Trying to initialize klog, when it is already initialized\n");
        return false;
    }

    if (max_number_loggers == 0) {
        printf("Trying to initialize klog with maximum number of loggers set to 0");
        return false;
    }

    if (logger_name_max_length == 0) {
        printf("Trying to initialize klog with maximum length of logger names set to 0");
        return false;
    }

    if (message_queue_size == 0) {
        printf("Trying to initialize klog with maximum message queue size set to 0\n");
        return false;
    }

    if (message_max_length == 0) {
        printf("Trying to initialize klog with maximum length of messages set to 0\n");
        return false;
    }

    return true;
}

void klog_initialize_stdout(const klog_init_stdout_info_t* p_klog_init_stdout_info) {
    if (p_klog_init_stdout_info == NULL) {
        printf("No stdout sink provided\n");
        return;
    }
}

void klog_initialize_file(const klog_init_file_info_t* p_klog_init_file_info) {
    if (p_klog_init_file_info == NULL) {
        printf("No file sink provided\n");
        return;
    }
}

/* @todo kjk 2025/12/20 Implement for other (non linux) platforms */
pid_t klog_get_current_thread_id(void) {
    return syscall(SYS_gettid);
}

const char* klog_impl_get_level_string(const enum klog_level_e requested_level) {
    const uint32_t i_level_string = G_klog_level_string_length * requested_level;
    return &gp_klog_level_strings[i_level_string];
}

/* ================================================================================================================== */
/* API Implementation                                                                                                 */
/* ================================================================================================================== */

void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_size, const uint32_t message_max_length, const klog_init_stdout_info_t* p_klog_init_stdout_info, const klog_init_file_info_t* p_klog_init_file_info) {
    if (!klog_initialization_parameters_is_valid(max_number_loggers, logger_name_max_length, message_queue_size, message_max_length)) {
        exit(1);
    }

    g_klog_max_number_loggers = max_number_loggers;
    g_klog_logger_name_max_length = logger_name_max_length;

    /* set all characters to space (' ') by default, so we can auto fill the end of the names if they are too short */
    const uint32_t total_logger_names_array_size = g_klog_logger_name_max_length * g_klog_max_number_loggers;
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
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_OFF      * G_klog_level_string_length], off_string, strlen(off_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_CRITICAL * G_klog_level_string_length], critical_string, strlen(critical_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_ERROR    * G_klog_level_string_length], error_string, strlen(error_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_WARNING  * G_klog_level_string_length], warning_string, strlen(warning_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_INFO     * G_klog_level_string_length], info_string, strlen(info_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_DEBUG    * G_klog_level_string_length], debug_string, strlen(debug_string));
    memcpy(&gp_klog_level_strings[KLOG_LEVEL_TRACE    * G_klog_level_string_length], trace_string, strlen(trace_string));

    g_klog_current_number_loggers_created = 0;

    /* @todo kjk 2025/12/20 Create message queue */

    klog_initialize_stdout(p_klog_init_stdout_info);
    klog_initialize_file(p_klog_init_file_info);

    g_klog_is_initialized = true;
}

void klog_deinitialize(void) {
    if (!g_klog_is_initialized) {
        printf("Trying to de-initialize klog, when it is not yet initialized\n");
        exit(1);
    }

    g_klog_max_number_loggers = 0;
    g_klog_logger_name_max_length = 0;
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

    const uint32_t current_logger_handle = g_klog_current_number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_handle * g_klog_logger_name_max_length;
    const uint32_t number_chars_to_copy = strlen(logger_name) >= g_klog_logger_name_max_length ?
        g_klog_logger_name_max_length : /* copy as much as we can fit */
        strlen(logger_name);            /* copy it all - NOT including the null terminator (which strlen doesn't count anyways) */
    memcpy(&gp_klog_logger_names[logger_name_start_index], logger_name, number_chars_to_copy);

    gp_klog_logger_levels[current_logger_handle] = KLOG_LEVEL_OFF;

    g_klog_current_number_loggers_created++;

    const klog_logger_handle_t handle = {current_logger_handle};
    return handle;
}

void klog_logger_set_level(const klog_logger_handle_t logger_handle, const enum klog_level_e updated_level) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        printf("Trying to set level for logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    gp_klog_logger_levels[logger_handle.value] = updated_level;
}

void klog(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle.value >= g_klog_current_number_loggers_created) {
        printf("Trying to log with logger %d, when only %d loggers exist\n", logger_handle.value, g_klog_current_number_loggers_created);
        exit(1);
    }

    if (requested_level > gp_klog_logger_levels[logger_handle.value]) {
        return;
    }

    va_list args;
    va_start(args, format);
    /* +1 for null termination */
    const int total_length = vsnprintf(0, 0, format, args) + 1;
    va_end(args);
    char* resulting_string = malloc(total_length);
    va_start(args, format);
    vsnprintf(resulting_string, total_length, format, args);
    va_end(args);

    const pid_t thread_id = klog_get_current_thread_id();
    const char* logger_name = &gp_klog_logger_names[logger_handle.value * g_klog_logger_name_max_length];
    const char* level_string = klog_impl_get_level_string(requested_level);

    printf("%5d [%.*s] [%.*s] %s\n", thread_id, g_klog_logger_name_max_length, logger_name, G_klog_level_string_length, level_string, resulting_string);

    free(resulting_string);
}
