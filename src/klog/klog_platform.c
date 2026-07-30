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
    klog_platform_mutex_t* p_mutex
) {
    int result = pthread_mutex_init(p_mutex, NULL); /* "pthread_mutex_init() always returns 0" - from the man page ????? */
    if (result == EBUSY) {
        kdprintf("pthread_mutex_init returned %d: Mutex is already initialized\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EAGAIN) {
        kdprintf("pthread_mutex_init returned %d: Insufficient resources to initialize another mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == ENOMEM) {
        kdprintf("pthread_mutex_init returned %d: Insufficient memory exists to initialize another mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EINVAL) {
        kdprintf("pthread_mutex_init returned %d: Attributes are invalid\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EPERM) {
        kdprintf(
            "pthread_mutex_init returned %d: Caller doesn't have permission for the operation\n",
            result
        );
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_mutex_deinitialize(
    klog_platform_mutex_t* p_mutex
) {
    int result = pthread_mutex_destroy(p_mutex);
    if (result == EBUSY) {
        kdprintf("pthread_mutex_destroy returned %d: Attempt to destroy mutex while it is locked\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EINVAL) {
        kdprintf("pthread_mutex_destroy returned %d: The mutex specified is invalid\n", result);
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_mutex_lock(
    klog_platform_mutex_t* p_mutex
) {
    int result = pthread_mutex_lock(p_mutex);
    if (result == EINVAL) {
        kdprintf("pthread_mutex_lock returned %d: Mutex is not initialized\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EBUSY) {
        kdprintf("pthread_mutex_lock returned %d: Mutex couldn't be acquired because it was already locked\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EAGAIN) {
        kdprintf("pthread_mutex_lock returned %d: Maximum number of recursive locks has been exceeded\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EDEADLK) {
        kdprintf("pthread_mutex_lock returned %d: Deadlock condition was detected, or current thread already owns mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EPERM) {
        kdprintf("pthread_mutex_lock returned %d: Current thread does not own the mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_mutex_unlock(
    klog_platform_mutex_t* p_mutex
) {
    int result = pthread_mutex_unlock(p_mutex);
    if (result == EINVAL) {
        kdprintf("pthread_mutex_unlock returned %d: Mutex is not initialized\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EBUSY) {
        kdprintf("pthread_mutex_unlock returned %d: Mutex is already locked\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EAGAIN) {
        kdprintf("pthread_mutex_unlock returned %d: Maximum number of recursive locks has been exceeded\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EDEADLK) {
        kdprintf("pthread_mutex_unlock returned %d: Deadlock condition was detected, or current thread already owns mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EPERM) {
        kdprintf("pthread_mutex_unlock returned %d: Current thread does not own the mutex\n", result);
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_initialize(
    klog_platform_semaphore_t* p_semaphore,
    uint32_t                   count
) {
    int retval = sem_init(p_semaphore, 0, count);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_initialize: sem_init returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_deinitialize(
    klog_platform_semaphore_t* p_semaphore
) {
    const int retval = sem_destroy(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_deinitialize: sem_destroy returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_wait(
    klog_platform_semaphore_t* p_semaphore
) {
    const int retval = sem_wait(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_wait: sem_wait returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_semaphore_signal(
    klog_platform_semaphore_t* p_semaphore
) {
    const int retval = sem_post(p_semaphore);
    if (retval != 0) {
        kdprintf("Error in klog_platform_semaphore_signal: sem_post returned %d - errno: %s\n", retval, strerror(errno));
        exit(KLOG_EXIT_CODE);
    }
}

int klog_platform_semaphore_value_get(
    klog_platform_semaphore_t* p_semaphore
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
    klog_platform_thread_t* p_thread,
    void* (*                thread_body)(
        void*
    ),
    void*                   p_arg
) {
    int result = pthread_create(p_thread, NULL, thread_body, p_arg);
    if (result == EAGAIN) {
        kdprintf("pthread_create returned %d: Insufficient resources to create another thread\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EINVAL) {
        kdprintf("pthread_create returned %d: Invalid thread attribute settings\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EPERM) {
        kdprintf(
            "pthread_create returned %d: No permission to set the scheduling policy and parameters specified in thread attribute settings\n",
            result
        );
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_thread_join(
    klog_platform_thread_t* p_thread,
    void**                  p_ret
) {
    int result = pthread_join(*p_thread, p_ret);
    if (result == EDEADLK) {
        kdprintf("pthread_join returned %d: A deadlock was detected, or we are trying to join the calling thread (ourself)\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == EINVAL) {
        kdprintf("pthread_join returned %d: The thread we are trying to join is not a joinable thread\n", result);
        exit(KLOG_EXIT_CODE);
    }
    if (result == ESRCH) {
        kdprintf("pthread_join returned %d: No thread with the specified ID could be found\n", result);
        exit(KLOG_EXIT_CODE);
    }
}

void klog_platform_sleep_usec(
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
