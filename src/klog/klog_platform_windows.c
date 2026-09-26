#if defined(_WIN32)

#include "./klog_platform.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Windows.h>

#include "klog/klog.h"

procid_t klog_platform_get_current_thread_id(
    void
) {
    return GetCurrentThreadId();
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
    /**
     * @brief This was taken from https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows
     * @brief We probably could just sue the SYSTEMTIME field and not have to inoke the time(NULL)
     *      function for the remaining fields of our timepoint structure.
     */
    const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    const time_t now = time(NULL);

    SYSTEMTIME system_time;
    FILETIME   file_time;
    uint64_t   time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);

    time = (uint64_t)file_time.dwLowDateTime;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    const struct tm* const p_tm = localtime(&now);

    timepoint_t timepoint = {
        (time - EPOCH) / 10000000L,

        system_time.wMilliseconds * 1000,
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

#endif /* defined(_WIN32) */