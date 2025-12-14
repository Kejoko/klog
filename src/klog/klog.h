#ifndef KLOG_INCLUDED
#define KLOG_INCLUDED

/**
 * ------------------------------------------------------------------------
 * Requirements
 * ------------------------------------------------------------------------
 *
 * Log message structure
 * - Timestamp
 * - Thread ID
 * - Logging Silo
 * - Message
 *
 * Log sink structure
 * - All threads log to one file, with no clobbering
 *
 * API (compile time)
 * - set maximum verbosity
 *
 * API (runtime)
 * - initialize (can only happen once, program error if re-initialization attempt)
 *   - max number of threads
 *   - max number of items in the queue
 *   - format
 *   - sinks (file vs stdout)
 *     - filename
 *     - color
 * - create silo
 *   - name of silo, level
 * - log a message
 *   - silo
 *   - level
 *   - message
 *
 * Performance
 * - thread safe
 *
 * ------------------------------------------------------------------------
 * Implementation possiblities
 * ------------------------------------------------------------------------
 *
 * Possibility 1 - Multiple producers and single consumer
 * - Benefits
 *   - Threads only block when trying to submit to the queue
 *     - Blocking for queue submission is shorter than bloking for system resources for IO
 * - Drawbacks
 *   - Dedicated thread for consumer
 *     - Can be expensive if it is always polling the the queue of messages
 *
 * Possibility 2 - Producers only
 * - Mutex for each sink
 *   - Each thread has to acquire the mutex to output
 * - Benefits
 *   - Less threading
 *   - Simpler implementation
 * - Drawbacks
 *   - Blocking for IO will be very frequent, and blocking for system resource acquisition can be long
 *   - Will block once for each sync
 *
 * Possibility 3 - Producers only via threads
 * - Mutexes for each sync, new thread for each log
 * - Benefits
 *   - Main threads no longer block, because their logging threads will block instead
 * - Drawbacks
 *   - Many threads will be created and destroyed, lots of context switching overhead
 *
 * Possibility 4 - One output thread per processing thread
 * - Submit logs to the output thread, output thread handles all IO
 * - Benefits
 *   - Main threads do not block, because their logging threads will block instead
 * - Drawbacks
 *   - How do we determine which threads get a logging thread?
 *     - We don't want a logging thread for every single minor thread, only for the main ones
 *
 * ------------------------------------------------------------------------
 * Behavior (external view)
 * ------------------------------------------------------------------------
 *
 * ------------------------------------------------------------------------
 * Behavior (internal view)
 * ------------------------------------------------------------------------
 */

#include <stdint.h>

/*
#define KLOG_MAX_LOGGER_NAME_LENGTH 32
#define KLOG_MAX_NUMBER_LOGGERS 256
*/
#define KLOG_MAX_LOGGER_NAME_LENGTH 6
#define KLOG_MAX_NUMBER_LOGGERS 4

typedef uint32_t klog_logger_handle_t;

enum klog_level_e {
    KLOG_LEVEL_OFF = 0,
    KLOG_LEVEL_CRITICAL = 1,
    KLOG_LEVEL_ERROR = 2,
    KLOG_LEVEL_WARNING = 3,
    KLOG_LEVEL_INFO = 4,
    KLOG_LEVEL_DEBUG = 5,
    KLOG_LEVEL_TRACE = 6
};

void klog_initialize(void);
void klog_deinitialize(void);

klog_logger_handle_t    klog_logger_create(const char* logger_name);
void                    klog_logger_set_level(const klog_logger_handle_t logger_handle, const enum klog_level_e updated_level);

void klog_log(const klog_logger_handle_t logger_handle, const enum klog_level_e level, const char* format, ...);

#endif // KLOG_INCLUDED
