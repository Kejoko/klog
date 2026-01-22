#ifndef KLOG_INITIALIZE_INCLUDED
#define KLOG_INITIALIZE_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#include "klog/klog.h"

bool klog_initialize_are_parameters_valid(const bool klog_is_initialized, const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_max_length);

KlogLoggerHandle* klog_initialize_logger_handle_array(const uint32_t max_number_loggers);
char*             klog_initialize_logger_names_buffer(const uint32_t max_number_loggers, const uint32_t logger_name_max_length);
uint8_t*          klog_initialize_logger_levels_array(const uint32_t max_number_loggers);
char*             klog_initialize_level_strings_buffer(void);
char*             klog_initialize_message_queue(const uint32_t message_queue_number_elements, const uint32_t message_max_length);

void klog_initialize_stdout(const KlogInitStdoutInfo* p_klog_init_stdout_info);
void klog_initialize_file(const KlogInitFileInfo* p_klog_init_file_info);

#endif /* KLOG_INITIALIZE_INCLUDED */
