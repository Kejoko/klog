/**
 * @brief Ensure that the logger's level is set accordingly
 *      - Ensure that the logger levels array is updated accordingly
 *      - Ensure that when we log, the message is filtered accordingly
 *          - Log to a tempfile, and ensure the logs with insufficient level are not logged
 */

#include <stdlib.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_handle.h"

int set_levels(void) {
    const uint32_t max_number_loggers = 100;
    const uint32_t max_name_length = 3;
    klog_initialize(max_number_loggers, (KlogFormatInfo){max_name_length, 10, 0, false, false}, NULL, NULL, NULL);

    for (uint32_t i = 0; i < max_number_loggers; ++i) {
        const uint32_t desired_level = i % 7; /* @todo kjk 2026/02/10 Get rid of hardcoded number of levels */

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

int noop(void) {
    return 0;
}

int main(void) {
    return
        set_levels() ||
        noop()
    ;
}
