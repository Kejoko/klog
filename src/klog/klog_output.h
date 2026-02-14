#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./klog_format.h"

void klog_output_console(const KlogString* p_prefix, const KlogString* p_message);
void klog_output_file(FILE* const p_file, const KlogString* p_prefix, const KlogString* p_message);

#endif /* KLOG_OUTPUT_INCLUDED */
