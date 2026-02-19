#include "./klog_debug_util.h"

#include <stdarg.h>
#include <stdio.h>

void kdprintf(const char* const s_format, ...) {
#ifdef KLOG_DEBUG
    va_list p_args;
    va_start(p_args, s_format);
    vprintf(s_format, p_args);
    va_end(p_args);
#else
    (void)s_format;
#endif
}
