#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "klog/klog.h"

#include "../klog_initialize.h"

int already_initialized(
    void
) {
    if (klog_initialize_are_parameters_valid(true, 0, (KlogFormatInfo) { 0 }, NULL, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when klog is already initialized\n");
        return 1;
    }

    return 0;
}

int no_loggers(
    void
) {
    if (klog_initialize_are_parameters_valid(false, 0, (KlogFormatInfo) { 0 }, NULL, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum number of loggers is set to 0\n");
        return 1;
    }

    return 0;
}

int no_logger_name_length(
    void
) {
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 0, 100, 0, false, false }, NULL, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum length of logger names is set to 0\n");
        return 1;
    }

    return 0;
}

int no_message_length(
    void
) {
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, NULL, NULL)) {
        printf("Parameters should be invalid when the maximum length of a message is set to 0\n");
        return 1;
    }

    return 0;
}

int no_alloc_cb(
    void
) {
    KlogAllocInfo alloc_info = { NULL, &free };
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, NULL, &alloc_info)) {
        printf("Parameters should be invalid when KlogAllocInfo pointer is provided but alloc_cb is NULL\n");
        return 1;
    }

    return 0;
}

int no_free_cb(
    void
) {
    KlogAllocInfo alloc_info = { &malloc, NULL };
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, NULL, &alloc_info)) {
        printf("Parameters should be invalid when KlogAllocInfo pointer is provided but free_cb is NULL\n");
        return 1;
    }

    return 0;
}

int valid(
    void
) {
    if (!klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 100, 0, false, false }, NULL, NULL, NULL, NULL)) {
        printf("Parameters should be valid - case 0\n");
        return 1;
    }

    KlogAllocInfo alloc_info = { &malloc, &free };
    if (!klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 100, 0, false, false }, NULL, NULL, NULL, &alloc_info)) {
        printf("Parameters should be valid - case 1\n");
        return 1;
    }

    return 0;
}

int noop(
    void
) {
    return 0;
}

int main(
    void
) {
    const int result = already_initialized()
        || no_loggers()
        || no_message_length()
        || no_message_length()
        || no_alloc_cb()
        || no_free_cb()
        || valid()
        || noop()
    ;

    return result;
}
