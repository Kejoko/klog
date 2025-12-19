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

/**
 * @typedef klog_logger_handle_t The handle to a klog logger
 */
typedef uint32_t klog_logger_handle_t;

/**
 * @enum klog_level_e The different levels of verbosity
 */
enum klog_level_e {
    KLOG_LEVEL_OFF = 0,
    KLOG_LEVEL_CRITICAL = 1,
    KLOG_LEVEL_ERROR = 2,
    KLOG_LEVEL_WARNING = 3,
    KLOG_LEVEL_INFO = 4,
    KLOG_LEVEL_DEBUG = 5,
    KLOG_LEVEL_TRACE = 6
};

/**
 * @fn klog_initialize
 * @brief
 * @details
 * @pre klog has not been initialized.
 * @param max_number_loggers The maximum number of loggers able to be created
 * @param logger_name_max_length The maximum length of a logger's name. This
 *      also determines the width of the logger names when printing. A max
 *      length of 8 will print 8 characters for each logger name, where it
 *      will be left-justified and filled with spaces if the name is not long
 *      enough to fill all characters.
 */
void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length);

/**
 * @fn klog_deinitialize
 * @brief Deinitialize klog
 * @details
 * @pre klog has been initialized.
 */
void klog_deinitialize(void);

/**
 * @fn klog_logger_create
 * @todo Use the hash of the logger_name to compare against already created
 *      loggers, and store the truncated version. This will allow us to have
 *      "duplicate" truncated versions, resulting from differing full names.
 *      This will require us to store the hash of each logger in memory too.
 * @brief Create a handle to a logger for the given name
 * @details If a logger does not exist using the given name, create one and
 *      return the corresponding handle. If a logger with the given name does
 *      exist, return a handle for that logger instead. If the requested name's
 *      length exceeds KLOG_MAX_LOGGER_NAME_LENGTH, it will be truncated to be
 *      the first KLOG_MAX_LOGGER_NAME_LENGTH characters. If the truncated
 *      version of the given name matches the truncated version of an existing
 *      logger, a handle to the existing logger with a truncated name will be
 *      returned.
 * @pre klog has been initialized.
 * @pre There exists fewer than KLOG_MAX_NUMBER_LOGGERS, if a logger for the
 *      given name does not exist.
 * @param logger_name The name of the logger to create
 * @returns klog_logger_handle_t The newly created handle if logger_name did
 *      not already exist, or the retrieved handle if logger_name did exist
 */
klog_logger_handle_t klog_logger_create(const char* logger_name);

/**
 * @fn klog_logger_set_level
 * @brief Set the maximum verbosity level allowed for a given logger
 * @details This updates the current level of verbosity for a logger such that
 *      any log messages which are more verbose than the current level will
 *      be filtered out and not logged. If you set the level to
 *      info, then debug and trace logs will be filtered out. If you set the
 *      level to critical, only critical logs will be enabled. Etc.
 * @pre klog has been initialized.
 * @pre logger_handle is valid.
 * @param logger_handle The handle to the logger
 * @param updated_level The desired level of verbosity
 */
void klog_logger_set_level(const klog_logger_handle_t logger_handle, const enum klog_level_e updated_level);

/**
 * @fn klog
 * @brief
 * @details
 * @pre klog has been initialized.
 * @pre logger_handle is valid.
 * @param logger_handle
 * @param requested_level 
 * @param format
 * @param ...
 */
void klog(const klog_logger_handle_t logger_handle, const enum klog_level_e requested_level, const char* format, ...);

#endif // KLOG_INCLUDED
