#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "./klog_state.h"

/* ================================================================================================================== */
/* Klog globals                                                                                                       */
/* ================================================================================================================== */

/* Set by the user -------------------------------------------------------------------------------------------------- */

uint32_t g_klog_max_number_loggers = 0;

uint32_t g_klog_logger_name_max_length = 0;

uint32_t g_klog_number_backing_threads = 0;

uint32_t g_klog_message_queue_number_elements = 0;

bool     g_klog_print_timestamp = false;

bool     g_klog_print_source_location = false;

uint32_t g_klog_message_max_length = 0;

/* Internal state --------------------------------------------------------------------------------------------------- */

uint32_t                 g_klog_current_number_loggers_created = 0;

struct KlogLoggerHandle* ga_klog_logger_handles = NULL;

char*                    gp_klog_logger_names = NULL;

uint8_t*                 ga_klog_logger_levels = NULL;

char*                    gb_klog_level_strings = NULL;

char*                    gb_klog_message_queue = NULL;

FILE*                    gp_klog_output_file = NULL;

bool                     g_klog_is_initialized = false;
