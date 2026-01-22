#include "./klog_output.h"

#include <stdio.h>

void klog_output_stdout(char* message) {
    printf("%s\n", message);
}

void klog_output_file(FILE* p_file, char* message) {
    if (!p_file) {
        return;
    }
    fprintf(p_file, "%s\n", message);
}

