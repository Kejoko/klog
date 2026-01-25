#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./klog_format.h"

void klog_output_stdout(const uint32_t* thread_id, KlogString* p_time, KlogString* p_name, KlogString* p_level, KlogString* p_source_location, KlogString* p_message);
void klog_output_file(FILE* p_file, const uint32_t* thread_id, KlogString* p_time, KlogString* p_name, KlogString* p_level, KlogString* p_source_location, KlogString* p_message);

#endif /* KLOG_OUTPUT_INCLUDED */
