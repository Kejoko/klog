#ifndef KLOG_INITIALIZE_INCLUDED
#define KLOG_INITIALIZE_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "klog/klog.h"

bool klog_initialize_are_parameters_valid(
    const bool             klog_is_initialized,
    const uint32_t         max_number_loggers,
    const KlogFormatInfo   klog_format_info,
    const KlogAsyncInfo*   p_klog_async_info,
    const KlogConsoleInfo* p_klog_console_info,
    const KlogFileInfo*    p_klog_file_info,
    const KlogAllocInfo*   p_klog_alloc_info
);

char* klog_initialize_buffer(
    const uint32_t number_elements,
    const uint32_t element_length_max,
    const char     fill_char,
    const bool     null_terminate,
    void* (* const cb_alloc)(
        size_t size
    )
);

KlogLoggerHandle* klog_initialize_logger_handle_array(
    const uint32_t max_number_loggers,
    void* (* const cb_alloc)(
        size_t size
    )
);

uint8_t* klog_initialize_logger_levels_array(
    const uint32_t max_number_loggers,
    void* (* const cb_alloc)(
        size_t size
    )
);

char* klog_initialize_level_strings_buffer(
    void* (*const cb_alloc)(
        size_t size
    )
);

char* klog_initialize_colored_level_strings_buffer(
    void* (*const cb_alloc)(
        size_t size
    )
);

FILE* klog_initialize_file(
    const KlogFileInfo* p_klog_file_info,
    void* (* const      cb_alloc)(
        size_t size
    )
);

#endif /* KLOG_INITIALIZE_INCLUDED */
