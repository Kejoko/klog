#include "./klog_platform.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "klog/klog.h"

#include "./klog_debug_util.h"

procid_t klog_platform_get_current_thread_id(
    void
) {
    return syscall(SYS_gettid);
}

void klog_platform_mutex_initialize(
    kpl_mutex_t* p_mutex
) {
    pthread_mutex_init(p_mutex, NULL);
}

void klog_platform_mutex_deinitialize(
    kpl_mutex_t* p_mutex
) {
    pthread_mutex_destroy(p_mutex);
}

void klog_platform_mutex_lock(
    kpl_mutex_t* p_mutex
) {
    pthread_mutex_lock(p_mutex);
}

void klog_platform_mutex_unlock(
    kpl_mutex_t* p_mutex
) {
    pthread_mutex_unlock(p_mutex);
}

void klog_platform_semaphore_initialize(
    kpl_semaphore_t* p_semaphore,
    uint32_t         count
) {
    int retval = sem_init(p_semaphore, 0, count);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_initialize: sem_init returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_deinitialize(
    kpl_semaphore_t* p_semaphore
) {
    const int retval = sem_destroy(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_deinitialize: sem_destroy returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_wait(
    kpl_semaphore_t* p_semaphore
) {
    const int retval = sem_wait(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_wait: sem_wait returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_signal(
    kpl_semaphore_t* p_semaphore
) {
    const int retval = sem_post(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_signal: sem_post returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

int klog_platform_semaphore_value_get(
    kpl_semaphore_t* p_semaphore
) {
    int       result = 0;
    const int retval = sem_getvalue(p_semaphore, &result);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_value_get: sem_getvalue returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
    return result;
}

void klog_platform_thread_create(
    kpl_thread_t* p_thread,
    void* (*      thread_body)(
        void*
    ),
    void*         p_arg
) {
    pthread_create(p_thread, NULL, thread_body, p_arg);
}

void klog_platform_thread_join(
    kpl_thread_t* p_thread,
    void**        p_ret
) {
    pthread_join(*p_thread, p_ret);
}

void sleep_usec(
    const uint32_t usec
) {
    struct timespec ts;
    ts.tv_sec  = usec / 1000000;          /* Whole seconds */
    ts.tv_nsec = (usec % 1000000) * 1000; /* Remaining nanoseconds */
    nanosleep(&ts, NULL);
}

const char* klog_platform_get_basename(
    const char* const s_filepath
) {
    return basename((char*)s_filepath);
}

timepoint_t klog_platform_get_current_timepoint(
    void
) {
    const time_t   now = time(NULL);
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        kdprintf("Failure when invoking gettimeofday()\n");
        exit(KLOG_EXIT_CODE);
    }
    const struct tm* const p_tm = localtime(&now);

    timepoint_t timepoint = {
        tv.tv_sec,

        tv.tv_usec,
        p_tm->tm_sec,
        p_tm->tm_min,
        p_tm->tm_hour,
        p_tm->tm_mday,
        p_tm->tm_mon + 1,
        p_tm->tm_year + 1900,

        p_tm->tm_wday + 1,
        p_tm->tm_yday + 1,

        p_tm->tm_isdst
    };

    return timepoint;
}
