#ifndef KLOG_STATE_INCLUDED
#define KLOG_STATE_INCLUDED

/**
 * @brief The only implementation file which can include this is klog.c, though
 *      the unit test files may also include this file so they can inspect the
 *      state after certain operations to ensure correctness
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* ================================================================================================================== */
/* Klog globals                                                                                                       */
/* ================================================================================================================== */

/* Set by the user -------------------------------------------------------------------------------------------------- */

/*  The maximum number of loggers allowed */
extern uint32_t g_klog_max_number_loggers;

/* The maximum length allowed for a logger's name */
extern uint32_t g_klog_logger_name_max_length;

/* The number of backing logging threads we have */
extern uint32_t g_klog_number_backing_threads;

/* The number of messages in the message queue */
extern uint32_t g_klog_message_queue_number_elements;

/* Whether or not to print a timestamp */
extern bool     g_klog_print_timestamp;

/* How the long the filename should be when printing source location. If 0, then source location is not printed */
extern uint32_t g_klog_source_location_filename_max_length;

/* The length of messages in the queue */
extern uint32_t g_klog_message_max_length;

/* Internal state --------------------------------------------------------------------------------------------------- */

/* The current number of loggers which actually exist */
extern uint32_t                 g_klog_current_number_loggers_created;

/* The actual logger handles */
extern struct KlogLoggerHandle* ga_klog_logger_handles;

/* Array of logger names, non-null terminated */
extern char*                    gp_klog_logger_names;

/* Array of uint8_t for the current allowed level for each logger */
extern uint8_t*                 ga_klog_logger_levels;

/* Array of the logging levels stringified in a nicely printable format, all fixed width. Non-null terminated */
extern char*                    gb_klog_level_strings;

/* Array of the logging levels (with color) stringified in a nicely printable format, all fixed width. Non-null terminated */
extern char*                    gb_klog_colored_level_strings;

/* The queue of messages */
extern char*                    gb_klog_message_queue;

/* The file we are outputting to */
extern FILE*                    gp_klog_output_file;

/* Whether or not klog has been initialized */
extern bool                     g_klog_is_initialized;

#endif /* KLOG_STATE_INCLUDED */
