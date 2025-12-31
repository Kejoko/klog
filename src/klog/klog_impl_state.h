#ifndef KLOG_IMPL_STATE_INCLUDED
#define KLOG_IMPL_STATE_INCLUDED

#include <stdbool.h>
#include <stdint.h>

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

/* The length of messages in the queue */
extern uint32_t g_klog_message_max_length;

/* Internal state --------------------------------------------------------------------------------------------------- */

/* The current number of loggers which actually exist */
extern uint32_t g_klog_current_number_loggers_created;

/* Array of logger names, non-null terminated */
extern char* gp_klog_logger_names;

/* Array of uint8_t for the current allowed level for each logger */
extern uint8_t* gp_klog_logger_levels;

/* Array of the logging levels stringified in a nicely printable format, all fixed width. Non-null terminated */
extern char* gp_klog_level_strings;

/* The queue of messages */
extern char* gp_klog_message_queue;

/* Whether or not klog has been initialized */
extern bool g_klog_is_initialized;

/* ================================================================================================================== */
/* Constants                                                                                                          */
/* ================================================================================================================== */

extern const uint32_t G_klog_level_string_length;
extern const uint32_t G_klog_number_levels;

#endif /* KLOG_IMPL_STATE_INCLUDED */
