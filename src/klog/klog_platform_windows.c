#if defined(_WIN32)

#include "./klog_platform.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

procid_t klog_platform_get_current_thread_id(
    void
) {
    return 0;
}

void klog_platform_mutex_initialize(
    klog_platform_mutex_t* p_mutex
) {
    (void)p_mutex;
}

void klog_platform_mutex_deinitialize(
    klog_platform_mutex_t* p_mutex
) {
    (void)p_mutex;
}

void klog_platform_mutex_lock(
    klog_platform_mutex_t* p_mutex
) {
    (void)p_mutex;
}

void klog_platform_mutex_unlock(
    klog_platform_mutex_t* p_mutex
) {
    (void)p_mutex;
}

void klog_platform_semaphore_initialize(
    klog_platform_semaphore_t* p_semaphore,
    uint32_t                   count
) {
    (void)p_semaphore;
    (void)count;
}

void klog_platform_semaphore_deinitialize(
    klog_platform_semaphore_t* p_semaphore
) {
    (void)p_semaphore;
}

void klog_platform_semaphore_wait(
    klog_platform_semaphore_t* p_semaphore
) {
    (void)p_semaphore;
}

void klog_platform_semaphore_signal(
    klog_platform_semaphore_t* p_semaphore
) {
    (void)p_semaphore;
}

int klog_platform_semaphore_value_get(
    klog_platform_semaphore_t* p_semaphore
) {
    (void)p_semaphore;
}

void klog_platform_thread_create(
    klog_platform_thread_t* p_thread,
    void* (*thread_body)(
        void*
        ),
    void* p_arg
) {
    (void)p_thread;
    (void)thread_body;
    (void)p_arg;
}

void klog_platform_thread_join(
    klog_platform_thread_t* p_thread,
    void** p_ret
) {
    (void)p_thread;
    (void)p_ret;
}

void klog_platform_sleep_usec(
    uint32_t usec
) {
    (void)usec;
}

const char* klog_platform_get_basename(
    const char* const s_filepath
) {
    return s_filepath;
}

timepoint_t klog_platform_get_current_timepoint(
    void
) {
    timepoint_t timepoint;
    return timepoint;
}

#endif /* defined(_WIN32) */