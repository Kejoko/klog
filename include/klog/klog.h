#ifndef KLOG_INCLUDED
#define KLOG_INCLUDED

#include <stdbool.h>
#include <stdint.h>

/**
 * @struct KlogLoggerHandle The handle to a klog logger
 */
typedef struct KlogLoggerHandle KlogLoggerHandle;

typedef struct {
    uint32_t logger_name_max_length;
    uint32_t message_max_length;
    uint32_t source_location_filename_max_length;
    bool use_timestamp;
} KlogFormatInfo;

typedef struct {
    uint32_t message_queue_number_elements;
    uint32_t number_backing_threads;
} KlogAsyncInfo;

typedef struct {
    uint8_t max_level;
    bool use_color;
} KlogStdoutInfo;

typedef struct {
    uint8_t max_level;
    const char* filename_prefix;
} KlogFileInfo;

/**
 * @enum KlogLevel The different levels of verbosity
 */
enum KlogLevel {
    KLOG_LEVEL_OFF   = 0,
    KLOG_LEVEL_FATAL = 1,
    KLOG_LEVEL_ERROR = 2,
    KLOG_LEVEL_WARN  = 3,
    KLOG_LEVEL_INFO  = 4,
    KLOG_LEVEL_DEBUG = 5,
    KLOG_LEVEL_TRACE = 6,
    KLOG_LEVEL_COUNT = 7
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
void klog_initialize(const uint32_t max_number_loggers, const KlogFormatInfo klog_format_info, const KlogAsyncInfo* p_klog_async_info, const KlogStdoutInfo* p_klog_init_stdout_info, const KlogFileInfo* p_klog_init_file_info);

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
void klog_logger_set_level(const KlogLoggerHandle* p_logger_handle, const enum KlogLevel updated_level);

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
void klog_log(const KlogLoggerHandle* p_logger_handle, const enum KlogLevel requested_level, const char* file, const uint32_t line, const char* format, ...);

#define klog(p_logger_handle, requested_level, ...) klog_log(p_logger_handle, requested_level, __FILE__, __LINE__, __VA_ARGS__)
#define klog_trace(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define klog_debug(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define klog_info( p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define klog_warn( p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define klog_error(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define klog_fatal(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif /* KLOG_INCLUDED */
