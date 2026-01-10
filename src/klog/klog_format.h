#ifndef KLOG_FORMAT_INCLUDED
#define KLOG_FORMAT_INCLUDED

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
    const uint32_t logger_name_max_length;
    const char* p_logger_names;
    const char* p_level_strings;
} klog_format_context_t;

typedef struct {
    /* The number of resulting format strings */
    uint32_t  number_format_strings;

    /* An array of format strings, each index of this points to a format string */
    char**    format_strings;

    /* An array of lengths, so we know how long each of the format strings are */
    uint32_t* format_string_lengths;

    /* An array of indices, each representing where in va_args the corresponding format string should begin */
    uint32_t* va_arg_start_indices;

    /* An array of amount, each representing the amount of va_args the corresponding format string should use */
    uint32_t* va_arg_amount;
} klog_format_split_t;

pid_t klog_format_get_current_thread_id(void);

const char* klog_format_get_level_string(const klog_format_context_t context, const enum klog_level_e requested_level);

klog_format_split_t klog_format_split_strings(char* full_format);

char* klog_format(const klog_format_context_t context, const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, va_list args);

#endif /* KLOG_FORMAT_INCLUDED */
