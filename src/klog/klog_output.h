#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./klog_format.h"

void klog_output_stdout(const uint32_t* thread_id, const KlogString* p_time, const KlogString* p_name, const KlogString* p_level, const KlogString* p_source_location, const KlogString* p_message);
void klog_output_file(FILE* const p_file, const uint32_t* thread_id, const KlogString* p_time, const KlogString* p_name, const KlogString* p_level, const KlogString* p_source_location, const KlogString* p_message);

#endif /* KLOG_OUTPUT_INCLUDED */
