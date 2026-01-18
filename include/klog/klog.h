#ifndef KLOG_INCLUDED
#define KLOG_INCLUDED

/**
 * @todo kjk 2025/12/30 Document everything with Doxygen
 * @todo kjk 2025/12/30 Separate helper functions into their own header file so we can test them
 */

#include <stdbool.h>
#include <stdint.h>

/**
 * @struct KlogLoggerHandle The handle to a klog logger
 */
typedef struct {
    uint32_t value;
} KlogLoggerHandle;

typedef struct {
    uint8_t max_level;
    bool use_color;
} KlogInitStdoutInfo;

typedef struct {
    uint8_t max_level;
    const char* filename_prefix;
} KlogInitFileInfo;

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
 * @param p_stdout_init_info
 * @param p_file_init_info
 */
void klog_initialize(const uint32_t max_number_loggers, const uint32_t logger_name_max_length, const uint32_t message_queue_number_elements, const uint32_t message_max_length, const uint32_t number_backing_threads, const KlogInitStdoutInfo* p_klog_init_stdout_info, const KlogInitFileInfo* p_klog_init_file_info);

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
 * @todo Return a negative value upon failure instead of fully crashing the
 *      program. The program will crash when we try to print with a negative
 *      handle instead.
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
 * @returns KlogLoggerHandle The newly created handle if logger_name did
 *      not already exist, or the retrieved handle if logger_name did exist
 */
KlogLoggerHandle* klog_logger_create(const char* logger_name);

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
void klog_logger_set_level(const KlogLoggerHandle* p_logger_handle, const enum klog_level_e updated_level);

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
void klog(const KlogLoggerHandle* p_logger_handle, const enum klog_level_e requested_level, const char* format, ...);

#endif /* KLOG_INCLUDED */
