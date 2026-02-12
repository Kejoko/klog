/**
 * @brief Ensure that the logger's level is set accordingly
 *      - Ensure that the logger levels array is updated accordingly
 *      - Ensure that when we log, the message is filtered accordingly
 *          - Log to a tempfile, and ensure the logs with insufficient level are not logged
 */

#include <stdio.h>
#include <stdlib.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_handle.h"

#define KLOG_LEVEL_COUNT 7 /* @todo kjk 2026/02/10 Make this accessible everywhere */

int set_levels(void) {
    const uint32_t max_number_loggers = 100;
    const uint32_t max_name_length = 3;
    klog_initialize(max_number_loggers, (KlogFormatInfo){max_name_length, 10, 0, false, false}, NULL, NULL, NULL);

    for (uint32_t i = 0; i < max_number_loggers; ++i) {
        const uint32_t desired_level = i % KLOG_LEVEL_COUNT;

        char* s_name = malloc(max_name_length + 1);
        sprintf(s_name, "%.3d", i);

        const KlogLoggerHandle* p_handle = klog_logger_create(s_name);
        klog_logger_set_level(p_handle, desired_level);

        const uint32_t actual_level = g_klog_state.a_logger_levels[p_handle->value];
        if (actual_level != desired_level) {
            printf("Klog reports logger %d has level of %d instead of %d\n", p_handle->value, actual_level, desired_level);
            return 1;
        }

        free(s_name);
    }

    klog_deinitialize();

    return 0;
}

int log_levels_tempfile(void) {
    const uint32_t max_number_loggers = 100;
    const uint32_t max_name_length = 3;
    const KlogFileInfo file_info = { KLOG_LEVEL_TRACE, "/tmp/ut-klog_logger_set_level" }; /* @todo kjk 2026/02/11 portable tempfile path for windows and mac */
    klog_initialize(max_number_loggers, (KlogFormatInfo){max_name_length, 10, 0, false, false}, NULL, NULL, &file_info);

    const char* l_names[KLOG_LEVEL_COUNT] = { "001", "002", "003", "004", "005", "006", "007" };
    const KlogLoggerHandle* a_handles[KLOG_LEVEL_COUNT] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    /* So we can track the filesize and see if it changes as we log things */
    uint32_t file_size_previous = 0;

    /* i acts as the index to the handle, and also as the base accepted level for the handle */
    for (uint32_t i = 0; i < KLOG_LEVEL_COUNT; ++i) {
        a_handles[i] = klog_logger_create(l_names[i]);
        klog_logger_set_level(a_handles[i], i);

        /* Ensure that we cannot log something to the tempfile with a level that does not match or equal the base level */
        for (uint32_t i_log_level = 0; i_log_level < KLOG_LEVEL_COUNT; ++i_log_level) {
            if (i_log_level == 0) {
                continue; /* Logging with the off level is an error */
            }

            /* Try to log at the current level */
            klog(a_handles[i], i_log_level, "beep");

            /* Get the filesize */
            const uint32_t file_size_current = ftell(g_klog_state.p_file);

            /* If we should not have logged at this level, ensure the size did not change */
            if ((i_log_level > i) && (file_size_current != file_size_previous)) {
                printf("Logging at level %d when the set level is %d should not result in a change in file size\n", i_log_level, i);
                return 1;
            }

            /* If we SHOULD HAVE logged at this level, ensure the size DID change */
            if ((i_log_level <= i) && (file_size_current <= file_size_previous)) {
                printf("Logging at level %d when the set level is %d should result in an increased file size\n", i_log_level, i);
                return 1;
            }

            file_size_previous = file_size_current;
        }
    }

    klog_deinitialize();

    return 0;
}

int noop(void) {
    return 0;
}

int main(void) {
    return
        set_levels() ||
        log_levels_tempfile() ||
        noop()
    ;
}
