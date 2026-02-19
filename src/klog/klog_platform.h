#ifndef KLOG_PLATFORM_INCLUDED
#define KLOG_PLATFORM_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#error "Klog does not support Windows"
#endif

#ifdef __APPLE__
#error "Klog does not support MacOS"
#endif

/* ================================================================================================================== */
/* Linux                                                                                                              */
/* ================================================================================================================== */

#ifdef __linux__

/* Thread / Process ID ---------------------------------------------------------------------------------------------- */

#include <unistd.h> /* For pid_t */
#include <sys/syscall.h> /* For syscall() */

#ifndef SYS_gettid
#error "SYS_gettid is unavailable on this system"
#endif

typedef pid_t procid_t;

procid_t klog_platform_get_current_thread_id(void);

/* Filenames -------------------------------------------------------------------------------------------------------- */

#include <libgen.h> /* For basename() */

const char* klog_platform_get_basename(const char* const s_filepath);

/* Time ------------------------------------------------------------------------------------------------------------- */

#include <time.h>        /* For time(), localtime(), gettimeofday() */
#include <sys/time.h>

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

timepoint_t klog_platform_get_current_timepoint(void);

#endif

#endif /* KLOG_PLATFORM_INCLUDED */
