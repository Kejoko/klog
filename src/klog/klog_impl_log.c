#include "./klog_impl_log.h"

#include <stdio.h>

void klog_impl_log_stdout(char* message) {
    printf("%s\n", message);
}

void klog_impl_log_file(char* message) {
    printf("In file: %s\n", message);
}

