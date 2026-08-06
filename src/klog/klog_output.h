#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./klog_format.h"
#include "./klog_platform.h"

void klog_output(
    uint32_t               actual_message_length,
    const char*            s_message_formatted,
    KlogString             packed_prefix_console,
    KlogString             packed_prefix_file,
    uint8_t                level_requested,
    uint8_t                max_level_console,
    uint8_t                max_level_file,
    FILE*                  p_file,
    klog_platform_mutex_t* p_mutex_console,
    klog_platform_mutex_t* p_mutex_file
);

void klog_output_console(
    const KlogString* p_prefix,
    const KlogString* p_message
);

void klog_output_file(
    FILE* const       p_file,
    const KlogString* p_prefix,
    const KlogString* p_message
);

#endif /* KLOG_OUTPUT_INCLUDED */
