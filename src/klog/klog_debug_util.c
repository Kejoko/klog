#include "./klog_debug_util.h"

#include <stdarg.h>
#include <stdio.h>

void kdprintf(const char* format, ...) {
#ifdef KLOG_DEBUG
    va_list p_args;
    va_start(p_args, format);
    vprintf(format, p_args);
    va_end(p_args);
#else
    (void)format;
#endif
}
