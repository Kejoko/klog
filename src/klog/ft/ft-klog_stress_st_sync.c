/**
 * @brief A stress test to see how performative klog is without using async capabilities, in a single threaded application
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

#define NUM_LOGGERS 100
#define NUM_LOG_STATEMENTS (NUM_LOGGERS * 2000)

void do_test(void) {

    klog_initialize(NUM_LOGGERS, (KlogFormatInfo){3, 40, 10, false, false}, NULL, &(KlogConsoleInfo){KLOG_LEVEL_TRACE, false}, NULL);

    const KlogLoggerHandle* a_handles[NUM_LOGGERS];
    for (uint32_t i = 0; i < NUM_LOGGERS; ++i) { 
        char* const s_logger_name = malloc(4);
        sprintf(s_logger_name, "%.3d", i);

        a_handles[i] = klog_logger_create(s_logger_name);
        klog_logger_level_set(a_handles[i], KLOG_LEVEL_TRACE);

        free(s_logger_name);
    }

    for (uint32_t i = 0; i < NUM_LOG_STATEMENTS; ++i) {
        const uint32_t handle_index = i % NUM_LOGGERS;
        const uint32_t desired_level = (i % 6) + 1; /* @todo kjk 2025/01/06 Magic number 6 */

        const KlogLoggerHandle* handle = a_handles[handle_index];
        klog(handle, desired_level, "Log statement %.6d", i);
    }

    klog_deinitialize();
}

int main(void) {
    do_test();
    return 0;
}
