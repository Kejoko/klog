#include <stdbool.h>
#include <stdio.h>

#include "../klog_initialize.h"

int already_initialized(void) {
    if (klog_initialize_are_parameters_valid(true, 0, 0, 0)) {
        printf("Parameters should be invalid when klog is already initialized\n");
        return 1;
    }

    return 0;
}

int no_loggers(void) {
    if (klog_initialize_are_parameters_valid(false, 0, 0, 0)) {
        printf("Parameters should be invalid when the maximum number of loggers is set to 0\n");
        return 1;
    }

    return 0;
}

int no_logger_name_length(void) {
    if (klog_initialize_are_parameters_valid(false, 1, 0, 0)) {
        printf("Parameters should be invalid when the maximum length of logger names is set to 0\n");
        return 1;
    }

    return 0;
}

int no_message_length(void) {
    if (klog_initialize_are_parameters_valid(false, 1, 1, 0)) {
        printf("Parameters should be invalid when the maximum length of message is set to 0\n");
        return 1;
    }

    return 0;
}

int valid(void) {
    if (!klog_initialize_are_parameters_valid(false, 1, 1, 1)) {
        printf("Parameters should be valid\n");
        return 1;
    }

    return 0;
}

int noop(void) {
    return 0;
}

int main(void) {
    const int result =
        already_initialized() ||
        no_loggers() ||
        no_message_length() ||
        no_message_length() ||
        valid() ||
        noop()
    ;

    return result;
}
