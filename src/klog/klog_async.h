#ifndef KLOG_ASYNC_INCLUDED
#define KLOG_ASYNC_INCLUDED

#include <stdbool.h>

void* klog_async_thread_body(
    void* p
);

bool klog_async_consume(
    void
);

#endif /* KLOG_ASYNC_INCLUDED */
