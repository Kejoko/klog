#ifndef KLOG_INCLUDED
#define KLOG_INCLUDED

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief This is the code that klog will use if it invokes exit(), so you know
 *      where the invocation came from.
 */
#define KLOG_EXIT_CODE 32

/**
 * @brief This is the handle to a logger created via klog_logger_create(). You will
 *      need to provide this to all of the klog_logger_level_set() and klog()
 *      invocations.
 */
typedef struct KlogLoggerHandle KlogLoggerHandle;

/**
 * @enum KlogLevel The different levels of verbosity.
 */
enum KlogLevel {
    KLOG_LEVEL_OFF   = 0,
    KLOG_LEVEL_FATAL = 1,
    KLOG_LEVEL_ERROR = 2,
    KLOG_LEVEL_WARN  = 3,
    KLOG_LEVEL_INFO  = 4,
    KLOG_LEVEL_DEBUG = 5,
    KLOG_LEVEL_TRACE = 6
};

/**
 * @brief Formatting information
 * @var logger_name_length_max The maximum length of a logger's name. Any names longer than this will
 *      be truncated. Any names shorter than this will have spaces appended to meet this length.
 * @var message_length_max This is the maximum length (in characters) that a fully formatted message
 *      can be. Note that this length is enforced prior to the message being split at newlines. If you
 *      set this field to 10, and you have a message of length 15 with a newline at character index 5,
 *      you will get a line of 5 characters (0-4) and a line of 4 characters (6-9).
 * @var source_length_max The maximum length of the source file's filename. If this is set to 0, no
 *      source filename will be reported.
 * @var use_thread_id Whether or not to report the the id of the thread which submitted the message.
 * @var use_timestamp Whether or not to report the timestamp at which the message was submitted.
 */
typedef struct {
    uint32_t logger_name_length_max;
    uint32_t message_length_max;
    uint32_t source_length_max;
    bool     use_thread_id;
    bool     use_timestamp;
} KlogFormatInfo;

/**
 * @brief Async information
 * @note The overwrite_messages field is not currently supported. Support for this feature is planned
 *      for a future update.
 * @note If backing_thread_count is greater than 1, the ordering of the output messages is not
 *      guaranteed to be the same as the order of the inputs.
 * @var message_queue_element_count The size of the message queue. This determines how many messages
 *      can be un-consumed before klog will begin blocking upon submission of messages (if the
 *      overwrite_messages field is set to false).
 * @var backing_thread_count The number of dedicate consumer threads for klog to use.
 * @var overwrite_messages Whether or not the submitted messages should overwrite unconsumed messages
 *      if the message queue is full. If this is set to false, then klog will block until a message
 *      from the queue has been consumed.
 * @var discard_unconsumed Whether or not to discard unconsumed messages upon deinitialization of klog.
 *      If this is set to false, the deinitialization will block until all of the messages are consumed.
 */
typedef struct {
    uint32_t message_queue_element_count;
    uint32_t backing_thread_count;
    bool     overwrite_messages;
    bool     discard_unconsumed;
} KlogAsyncInfo;

/**
 * @brief Console information
 * @var level_max The maximum verbosity of messages that will print to the console.
 * @var use_color Whether or not to color-ize the levels of the logs for ease of parsing. This will
 *      make critical and error logs stand out due to red background and red foreground respectively,
 *      while the info logs will print with green, etc. See the example images in the readme for more
 *      info on coloring.
 */
typedef struct {
    uint8_t level_max;
    bool    use_color;
} KlogConsoleInfo;

/**
 * @brief File information
 * @var level_max The maximum verbosity of messages that will print to the console.
 * @var s_filename_prefix The prefix of the logfile that is created. This prefix will be appended
 *      with a string representing the timestamp at which the file was created, so you can reuse
 *      a prefix without destroying previous log files.
 */
typedef struct {
    uint8_t     level_max;
    const char* s_filename_prefix;
} KlogFileInfo;

/**
 * @brief User provided callbacks for custom allocation and free logic
 * @details This is useful is you have a custom allocator which you need to use. These allocation
 *      and free callbacks will be used in place of malloc and free respectively, if provided.
 * @var alloc_cb The callback used for allocating memory.
 * @var free_cb The callback used for freeing memory.
 */
typedef struct {
    void* (*alloc_cb)(
        size_t size
    );
    void (*free_cb)(
        void*
    );
} KlogAllocInfo;

/**
 * @fn klog_initialize
 * @brief Initialize klog
 * @details This accepts many parameters regarding the maximum number of things which will be used, for the
 *      sake of allocating all memory up front.
 * @note This is where klog allocates memory on the heap, according to the provided allocation callback.
 * @pre Klog is not currently initialized.
 * @pre All of the parameters contain valid values or are NULL.
 * @param logger_count_max    The maximum number of loggers that can be created.
 * @param klog_format_info
 * @param p_klog_async_info
 * @param p_klog_console_info
 * @param p_klog_file_info
 * @param p_klog_alloc_info
 */
void klog_initialize(
    const uint32_t         logger_count_max,
    const KlogFormatInfo   klog_format_info,
    const KlogAsyncInfo*   p_klog_async_info,
    const KlogConsoleInfo* p_klog_console_info,
    const KlogFileInfo*    p_klog_file_info,
    const KlogAllocInfo*   p_klog_alloc_info
);

/**
 * @fn klog_deinitialize
 * @brief De-initialize Klog
 * @details Clear the message queue, join all of the backing threads, free all of the memory.
 * @note This will block until all of the messages in the message queue have been logged, if the async information
 *      has the discard_unconsumed field set to false. If discard_unconsumed is set to true, the remaining messages
 *      will be cleared.
 * @note This is where klog "frees" (according to the provided free callback in the klog_initialize() invocation)
 *      the memory allocated on the heap.
 * @pre Klog is currently initialized.
 * @post Klog is not initialized.
 * @post All of Klog's backing threads have joined.
 */
void klog_deinitialize(
    void
);

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
 *      length exceeds the maximum allowed logger name length, it will be
 *      truncated. If the truncated version of the given name matches the
 *      truncated version of an existing logger, a handle to the existing logger
 *      with a truncated name will be returned.
 * @pre klog has been initialized.
 * @pre There exists fewer than the maximum configured number of loggers, if a
 *      logger for the given name does not exist.
 * @pre s_name does not contain invalid characters
 * @pre s_name is non-null
 * @pre strlen(s_name) is greater than 0
 * @pre name_length is greater than 0
 * @param s_name      The name of the logger to create.
 * @param name_length The length of the provided logger name, not including the
 *      null terminator.
 * @returns KlogLoggerHandle The newly created handle if logger_name did
 *      not already exist, or the retrieved handle if logger_name did exist
 */
const KlogLoggerHandle* klog_logger_create(
    const char* s_name,
    uint32_t    name_length
);

/**
 * @fn klog_logger_level_set
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
void klog_logger_level_set(
    const KlogLoggerHandle* p_logger_handle,
    const enum KlogLevel    updated_level
);

/**
 * @fn klog_initialize
 * @brief The underlying log functionality. You should prefer to use the abbreviated versions instead of this.
 * @details This is only exposed here so we can wrap it with quality of life macros to automatically populate
 *      the filename and line number arguments.
 * @note This is used under the hood for the abbreviated klog logging functionality. See klog, klog_trace,
 *      klog_debug, etc.
 * @pre Klog is initialized.
 * @pre The provided logger handle has been created.
 * @pre The requested level is not greater than KLOG_LEVEL_TRACE.
 * @param p_logger_handle The logger handle with which to log.
 * @param requested_level The requested level at which the message should be output.
 * @param s_filename      The filename in which this function was invoked.
 * @param line_number     The line number on which this function was invoked.
 * @param s_format        The printf style format string.
 * @param ...             The variadic arguments to populate the formatting string with.
 */
void klog_log(
    const KlogLoggerHandle* p_logger_handle,
    const enum KlogLevel    requested_level,
    const char*             s_filename,
    const uint32_t          line_number,
    const char*             s_format,
    ...
);

/**
 * @defgroup Klog logging interface
 */
/**@{*/

/**
 * @fn klog
 * @brief Log a message with the given logger at the desired level.
 * @details This is a wrapper around klog_log for quality of life to automatically have the line number
 *      and filename populated.
 * @note Prefer to use this instead of klog_log if you desire to log at a dynamic level.
 * @pre Klog is initialized.
 * @pre The provided logger handle has been created.
 * @pre The requested level is not greater than KLOG_LEVEL_TRACE.
 * @param p_logger_handle The logger handle with which to log.
 * @param requested_level The requested level at which the message should be output.
 * @param ...             The formatting string and variadic arguments.
 */
#define klog(p_logger_handle, requested_level, ...) klog_log(p_logger_handle, requested_level, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @fn klog_trace
 * @fn klog_debug
 * @fn klog_info
 * @fn klog_warn
 * @fn klog_error
 * @fn klog_fatal
 * @brief Log a message with the given logger at a fixed level according to the chosen invocation.
 * @details This is a wrapper around klog_log for quality of life to automatically have the line number
 *      and filename populated, and a fixed logging level for transparency and to reduce the number of
 *      required arguments.
 * @note Prefer to use this instead of klog_log if you desire to log at a dynamic level.
 * @pre Klog is initialized.
 * @pre The provided logger handle has been created.
 * @param p_logger_handle The logger handle with which to log.
 * @param ...             The formatting string and variadic arguments.
 */
/**@{*/
#define klog_trace(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define klog_debug(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define klog_info(p_logger_handle, ...)  klog_log(p_logger_handle, KLOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define klog_warn(p_logger_handle, ...)  klog_log(p_logger_handle, KLOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define klog_error(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define klog_fatal(p_logger_handle, ...) klog_log(p_logger_handle, KLOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)
/**@}*/

/**@}*/

#endif /* KLOG_INCLUDED */
