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

int no_message_queue_elements(
    void
) {
    KlogAsyncInfo async_info = { 0, 10, false, false };
    if (
        klog_initialize_are_parameters_valid(
            false,
            10,
            (KlogFormatInfo) { 10, 0, 0, false, false },
            &async_info,
            NULL,
            NULL,
            NULL
        )
    ) {
        printf("Parameters should be invalid when KlogAsyncInfo pointer is provided but message_queue_number_elements is 0\n");
        return 1;
    }

    return 0;
}

int no_backing_threads(
    void
) {
    KlogAsyncInfo async_info = { 10, 0, false, false };
    if (
        klog_initialize_are_parameters_valid(
            false,
            10,
            (KlogFormatInfo) { 10, 0, 0, false, false },
            &async_info,
            NULL,
            NULL,
            NULL
        )
    ) {
        printf("Parameters should be invalid when KlogAsyncInfo pointer is provided but number_backing_threads is 0\n");
        return 1;
    }

    return 0;
}

int invalid_filenames(
    void
) {
    char*        prefix    = calloc(2, 1);
    KlogFileInfo file_info = { KLOG_LEVEL_TRACE, prefix };

    prefix[0] = 7;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix contains character 7\n");
        return 1;
    }

    prefix[0] = 11;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix contains character 11\n");
        return 1;
    }

    prefix[0] = 12;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix contains character 12\n");
        return 1;
    }

    prefix[0] = 20;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix contains character 20\n");
        return 1;
    }

    prefix[0] = 127;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix contains character 127\n");
        return 1;
    }

    prefix[0] = '\0';
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix has length of 0\n");
        return 1;
    }

    free(prefix);

    file_info.s_filename_prefix = NULL;
    if (klog_initialize_are_parameters_valid(false, 10, (KlogFormatInfo) { 10, 0, 0, false, false }, NULL, NULL, &file_info, NULL)) {
        printf("Parameters should be invalid when KlogFileInfo's prefix is NULL\n");
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
        || no_logger_name_length()
        || no_message_length()
        || no_message_queue_elements()
        || invalid_filenames()
        || no_backing_threads()
        || no_alloc_cb()
        || no_free_cb()
        || valid()
        || noop()
    ;

    return result;
}
