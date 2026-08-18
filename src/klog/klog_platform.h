#ifndef KLOG_PLATFORM_INCLUDED
#define KLOG_PLATFORM_INCLUDED

#include <stdbool.h>
#include <stdint.h>

/* ================================================================================================================== */
/* Posix                                                                                                              */
/* ================================================================================================================== */

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)

/* Thread / Process ID ---------------------------------------------------------------------------------------------- */

# include <unistd.h>      /* For pid_t */
# include <sys/syscall.h> /* For syscall() */

# ifndef SYS_gettid
#  error "SYS_gettid is unavailable on this system"
# endif

typedef pid_t procid_t;

procid_t klog_platform_get_current_thread_id(
    void
);

/* Threading -------------------------------------------------------------------------------------------------------- */

# include <pthread.h>
# include <semaphore.h>

typedef pthread_mutex_t klog_platform_mutex_t;
typedef sem_t           klog_platform_semaphore_t;
typedef pthread_t       klog_platform_thread_t;

/* @todo Allow for mutex attributes */
void klog_platform_mutex_initialize(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_deinitialize(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_lock(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_unlock(
    klog_platform_mutex_t* p_mutex
);

/* @todo Allow for pshared parameter to denote inter-process sharing */
void klog_platform_semaphore_initialize(
    klog_platform_semaphore_t* p_semaphore,
    uint32_t                   count
);
void klog_platform_semaphore_deinitialize(
    klog_platform_semaphore_t* p_semaphore
);
void klog_platform_semaphore_wait(
    klog_platform_semaphore_t* p_semaphore
);
void klog_platform_semaphore_signal(
    klog_platform_semaphore_t* p_semaphore
);
int klog_platform_semaphore_value_get(
    klog_platform_semaphore_t* p_semaphore
);

/* @todo Allow for attributes upon thread creation */
void klog_platform_thread_create(
    klog_platform_thread_t* p_thread,
    void* (*                thread_body)(
        void*
    ),
    void*                   p_arg
);
void klog_platform_thread_join(
    klog_platform_thread_t* p_thread,
    void**                  p_ret
);

/* Microsecond sleep */
void klog_platform_sleep_usec(
    uint32_t usec
);

/* Filenames -------------------------------------------------------------------------------------------------------- */

# include <libgen.h> /* For basename() */

const char* klog_platform_get_basename(
    const char* const s_filepath
);

/* Time ------------------------------------------------------------------------------------------------------------- */

# include <time.h>       /* For time(), localtime(), gettimeofday() */
# include <sys/time.h>

/**
 * From struct tm:
 *  int    tm_sec   Seconds [0,60].
 *  int    tm_min   Minutes [0,59].
 *  int    tm_hour  Hour [0,23].
 *  int    tm_mday  Day of month [1,31].
 *  int    tm_mon   Month of year [0,11].
 *  int    tm_year  Years since 1900.
 *  int    tm_wday  Day of week [0,6] (Sunday =0).
 *  int    tm_yday  Day of year [0,365].
 *  int    tm_isdst Daylight Savings flag.
 *
 * From struct timeval:
 *  tv_sec  (seconds since j2k timestamp)
 *  tv_usec (microseconds)
 */

typedef struct {
    uint32_t second_j2k;

    uint32_t microsecond;
    uint32_t second;
    uint32_t minute;
    uint32_t hour;
    uint32_t day_month;
    uint32_t month;
    uint32_t year;

    uint32_t day_week;
    uint32_t day_year;

    bool is_daylight_savings;
} timepoint_t;

timepoint_t klog_platform_get_current_timepoint(
    void
);

#endif /* defined(__unix__) || defined(__linux__) || defined(__APPLE__) */

/* ================================================================================================================== */
/* Windows                                                                                                              */
/* ================================================================================================================== */

#if defined(_WIN32)

/* Thread / Process ID ---------------------------------------------------------------------------------------------- */

typedef uint32_t procid_t;

procid_t klog_platform_get_current_thread_id(
    void
);

/* Threading -------------------------------------------------------------------------------------------------------- */

typedef uint32_t klog_platform_mutex_t;
typedef uint32_t klog_platform_semaphore_t;
typedef uint32_t klog_platform_thread_t;

/* @todo Allow for mutex attributes */
void klog_platform_mutex_initialize(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_deinitialize(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_lock(
    klog_platform_mutex_t* p_mutex
);
void klog_platform_mutex_unlock(
    klog_platform_mutex_t* p_mutex
);

/* @todo Allow for pshared parameter to denote inter-process sharing */
void klog_platform_semaphore_initialize(
    klog_platform_semaphore_t* p_semaphore,
    uint32_t                   count
);
void klog_platform_semaphore_deinitialize(
    klog_platform_semaphore_t* p_semaphore
);
void klog_platform_semaphore_wait(
    klog_platform_semaphore_t* p_semaphore
);
void klog_platform_semaphore_signal(
    klog_platform_semaphore_t* p_semaphore
);
int klog_platform_semaphore_value_get(
    klog_platform_semaphore_t* p_semaphore
);

/* @todo Allow for attributes upon thread creation */
void klog_platform_thread_create(
    klog_platform_thread_t* p_thread,
    void* (*thread_body)(
        void*
        ),
    void* p_arg
);
void klog_platform_thread_join(
    klog_platform_thread_t* p_thread,
    void** p_ret
);

/* Microsecond sleep */
void klog_platform_sleep_usec(
    uint32_t usec
);

/* Filenames -------------------------------------------------------------------------------------------------------- */

const char* klog_platform_get_basename(
    const char* const s_filepath
);

/* Time ------------------------------------------------------------------------------------------------------------- */

typedef struct {
    uint32_t second_j2k;

    uint32_t microsecond;
    uint32_t second;
    uint32_t minute;
    uint32_t hour;
    uint32_t day_month;
    uint32_t month;
    uint32_t year;

    uint32_t day_week;
    uint32_t day_year;

    bool is_daylight_savings;
} timepoint_t;

timepoint_t klog_platform_get_current_timepoint(
    void
);

#endif /* defined(_WIN32) */

#endif /* KLOG_PLATFORM_INCLUDED */
