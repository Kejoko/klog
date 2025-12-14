#include "klog/klog.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

bool g_klog_is_initialized = false;

/* If this is >= max number loggers, error out and end program */
uint32_t g_klog_current_number_loggers_created = 0;

/* Array of logger names, each null terminated */
char* g_p_klog_logger_names = NULL;

/* Array of uint8_t for the current allowed level for each logger */
uint8_t* g_p_klog_logger_levels = NULL;

void klog_initialize(void) {
    /* +1 to allow for 32 characters and null termination */
    /* set all characters to space (' ') by default, so we can null terminate at a fixed length */
    const uint32_t total_logger_names_array_size = (KLOG_MAX_LOGGER_NAME_LENGTH + 1) * KLOG_MAX_NUMBER_LOGGERS;
    g_p_klog_logger_names = malloc(total_logger_names_array_size);
    memset(g_p_klog_logger_names, ' ', total_logger_names_array_size);

    const uint32_t total_logger_levels_array_size = KLOG_MAX_NUMBER_LOGGERS;
    g_p_klog_logger_levels = calloc(total_logger_levels_array_size, sizeof(g_p_klog_logger_levels));

    g_klog_current_number_loggers_created = 0;

    g_klog_is_initialized = true;
}

void klog_deinitialize(void) {
    g_klog_current_number_loggers_created = 0;
    free(g_p_klog_logger_names);
    free(g_p_klog_logger_levels);
    g_klog_is_initialized = false;
}

klog_logger_handle_t klog_logger_create(const char* logger_name) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (g_klog_current_number_loggers_created >= KLOG_MAX_NUMBER_LOGGERS) {
        printf("Trying to create klog logger, but klog only allows %d loggers\n", KLOG_MAX_NUMBER_LOGGERS);
        exit(1);
    }

    const klog_logger_handle_t current_logger_handle = g_klog_current_number_loggers_created;

    const uint32_t logger_name_start_index = current_logger_handle * (KLOG_MAX_LOGGER_NAME_LENGTH + 1);
    const uint32_t num_chars_to_copy = strlen(logger_name) > KLOG_MAX_LOGGER_NAME_LENGTH ?
        KLOG_MAX_LOGGER_NAME_LENGTH : /* copy as much as we can fit */
        strlen(logger_name);          /* copy it all */
    memcpy(&g_p_klog_logger_names[logger_name_start_index], logger_name, num_chars_to_copy);

    const uint32_t null_terminator_index = logger_name_start_index + KLOG_MAX_LOGGER_NAME_LENGTH;
    g_p_klog_logger_names[null_terminator_index] = '\0';

    const uint32_t logger_level_index = current_logger_handle;
    g_p_klog_logger_levels[logger_level_index] = KLOG_LEVEL_OFF;

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

    g_p_klog_logger_levels[logger_handle] = updated_level;
}

void klog_log(const klog_logger_handle_t logger_handle, const enum klog_level_e level, const char* format, ...) {
    if (!g_klog_is_initialized) {
        printf("Trying to create klog logger, but klog is not initialized\n");
        exit(1);
    }

    if (logger_handle >= g_klog_current_number_loggers_created) {
        printf("Trying to log with logger %d, when only %d loggers exist\n", logger_handle, g_klog_current_number_loggers_created);
        exit(1);
    }

    /* If we are logging at a level that is curretnly disallowed */
    if (level >= g_p_klog_logger_levels[logger_handle]) {
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

    printf("%s %d %s\n", &g_p_klog_logger_names[logger_handle * (KLOG_MAX_LOGGER_NAME_LENGTH + 1)], g_p_klog_logger_levels[logger_handle], resulting_string);

    free(resulting_string);
}
