#include "./klog_output.h"

#include <stdio.h>

void klog_output_stdout(char* message) {
    printf("%s\n", message);
}

void klog_output_file(char* message) {
    printf("In file: %s\n", message);
}

