#include "./klog_constants.h"

#include <stdint.h>

/* ================================================================================================================== */
/* Constants                                                                                                          */
/* ================================================================================================================== */

#define KLOG_LEVEL_STRING_LENGTH 5

const uint32_t G_klog_level_string_length = KLOG_LEVEL_STRING_LENGTH;

/* +5 for preceeding ansi sequence */
/* +4 for trailing ansi sequence (reset) */
const uint32_t G_klog_colored_level_string_length = (KLOG_LEVEL_STRING_LENGTH + 5 + 4);

const uint32_t G_klog_time_string_length = 19;

const uint32_t G_klog_number_levels = 7;
