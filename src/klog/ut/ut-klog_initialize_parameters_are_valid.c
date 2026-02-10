#include <stdbool.h>
#include <stdio.h>

#include "klog/klog.h"

#include "../klog_initialize.h"

int already_initialized(void) {
    if (klog_initialize_are_parameters_valid(true, 0, (KlogFormatInfo){0}, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when klog is already initialized\n");
        return 1;
    }

    return 0;
}

int no_loggers(void) {
    if (klog_initialize_are_parameters_valid(false, 0, (KlogFormatInfo){0}, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum number of loggers is set to 0\n");
        return 1;
    }

    return 0;
}

int no_logger_name_length(void) {
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo){0, 100, 0, false, false}, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum length of logger names is set to 0\n");
        return 1;
    }

    return 0;
}

int no_message_length(void) {
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo){10, 0, 0, false, false}, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum length of a message is set to 0\n");
        return 1;
    }

    return 0;
}

int valid(void) {
    if (!klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo){10, 100, 0, false, false}, NULL, NULL, NULL)) {
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
