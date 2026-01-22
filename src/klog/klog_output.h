#ifndef KLOG_OUTPUT_INCLUDED
#define KLOG_OUTPUT_INCLUDED

#include <stdio.h>

void klog_output_stdout(char* message);
void klog_output_file(FILE* p_file, char* message);

#endif /* KLOG_OUTPUT_INCLUDED */
