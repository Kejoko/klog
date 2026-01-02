#ifndef KLOG_IMPL_MESSAGE_CREATION_INCLUDED
#define KLOG_IMPL_MESSAGE_CREATION_INCLUDED

#include <stdarg.h>
#include <stdint.h>

#ifdef _WIN32
#error "Klog does not support Windows"
#endif

#ifdef __APPLE__
#error "Klog does not support MacOS"
#endif

#ifdef  __linux__
#include <unistd.h>
#include <sys/syscall.h>
#ifndef SYS_gettid
#error "SYS_gettid is unavailable on this system"
#endif
#endif

#include "klog/klog.h"

typedef struct {
    /* The number of resulting format strings */
    uint32_t  number_format_strings;

    /* An array of format strings, each index of this points to a format string */
    char**    format_strings;

    /* An array of indices, each representing where in va_args the corresponding format string should begin */
    uint32_t* va_arg_start_indices;

    /* An array of numbers, each representing the number of va_args the corresponding format string should use */
    uint32_t* va_arg_numbers;
} split_format_t;

pid_t klog_impl_get_current_thread_id(void);

const char* klog_impl_get_level_string(const enum klog_level_e requested_level);

split_format_t klog_impl_split_format_strings(char* full_format);

char* klog_impl_create_message(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, va_list args);

#endif /* KLOG_IMPL_MESSAGE_CREATION_INCLUDED */
