#ifndef KLOG_IMPL_INITIALIZATION_INCLUDED
#define KLOG_IMPL_INITIALIZATION_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#include "klog/klog.h"

bool klog_impl_parameters_are_valid(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length);

char*    klog_impl_create_logger_names_buffer(const uint32_t max_number_loggers, const uint32_t logger_name_max_length);
uint8_t* klog_impl_create_logger_levels_buffer(const uint32_t max_number_loggers);
char*    klog_impl_create_level_strings_buffer(void);
char*    klog_impl_create_message_queue(const uint32_t message_queue_number_elements, const uint32_t message_max_length);

void klog_impl_initialize_stdout(const klog_init_stdout_info_t* p_klog_init_stdout_info);
void klog_impl_initialize_file(const klog_init_file_info_t* p_klog_init_file_info);

#endif /* KLOG_IMPL_INITIALIZATION_INCLUDED */
