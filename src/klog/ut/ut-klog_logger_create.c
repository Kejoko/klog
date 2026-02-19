/**
 * @brief Ensure that the loggers are created correctly
 *      - Ensure that the number loggers created value is updated accordingly
 *      - Ensure that the logger handles array is updated accordingly
 *      - Ensure that the logger levels array is updated accordingly
 */

#include <stdlib.h>

#include "klog/klog.h"

#include "../klog_state.h"
#include "../klog_handle.h"

int create_single_logger(void) {
    const uint32_t max_number_loggers = 1;
    const uint32_t max_name_length = 3;
    klog_initialize(max_number_loggers, (KlogFormatInfo){max_name_length, 10, 0, false, false}, NULL, NULL, NULL);

    if (g_klog_state.number_loggers_max != max_number_loggers) {
        printf("Klog initialized with max number of loggers %d instead of %d\n", g_klog_state.number_loggers_max, max_number_loggers);
        return 1;
    }

    if (g_klog_state.number_loggers_created != 0) {
        printf("Klog initialized with %d loggers created instead of 0\n", g_klog_state.number_loggers_max);
        return 1;
    }

    const KlogLoggerHandle* p_handle_0 = klog_logger_create("ABC");
    if (g_klog_state.number_loggers_created != 1) {
        printf("Klog reports %d loggers created instead of 1\n", g_klog_state.number_loggers_max);
        return 1;
    }
    if (!p_handle_0) {
        printf("Klog produced invalid loggr handle\n");
        return 1;
    }

    klog_deinitialize();

    return 0;
}

int create_multiple_loggers(void) {
    const uint32_t max_number_loggers = 100;
    const uint32_t max_name_length = 3;
    klog_initialize(max_number_loggers, (KlogFormatInfo){max_name_length, 10, 0, false, false}, NULL, NULL, NULL);

    if (g_klog_state.number_loggers_max != max_number_loggers) {
        printf("Klog initialized with max number of loggers %d instead of %d\n", g_klog_state.number_loggers_max, max_number_loggers);
        return 1;
    }

    if (g_klog_state.number_loggers_created != 0) {
        printf("Klog initialized with %d loggers created instead of 0\n", g_klog_state.number_loggers_max);
        return 1;
    }

    for (uint32_t i = 0; i < max_number_loggers; ++i) {
        char* s_name = malloc(max_name_length + 1);
        sprintf(s_name, "%.3d", i);

        const KlogLoggerHandle* p_handle = klog_logger_create(s_name);
        
        if (g_klog_state.number_loggers_created != (i+1)) {
            printf("Klog reports %d loggers created instead of %d\n", g_klog_state.number_loggers_max, i+1);
            return 1;
        }
        if (!p_handle) {
            printf("Klog produced invalid loggr handle\n");
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
        create_single_logger() ||
        create_multiple_loggers() ||
        noop()
    ;
}
