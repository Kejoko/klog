#ifndef KLOG_CONSTANTS_INCLUDED
#define KLOG_CONSTANTS_INCLUDED

#include <stdint.h>

/* ================================================================================================================== */
/* Constants                                                                                                          */
/* ================================================================================================================== */

/**
 * @brief The length of the string representing the current level.
 * @note This is always used for file logging, and is used for console
 *      logging when we are not using colored levels.
 */
extern const uint32_t G_klog_level_string_length;

/**
 * @brief The length of the string representing the current level
 *      when using colors.
 * @note This is only used when console logging with colored logs
 *      enabled.
 */
extern const uint32_t G_klog_colored_level_string_length;

/**
 * @brief The length of the string representing the timestamp.
 */
extern const uint32_t G_klog_time_string_length;

/**
 * @brief The number of different logging levels we have available.
 * @note This is so we don't have to use the greatest value of the
 *      KlogLevel enumeration (currently KLOG_LEVEL_TRACE), allowing
 *      us to add and change the order of levels without required the
 *      code which is dependent upon the number of levels to change.
 */
extern const uint32_t G_klog_number_levels;

#endif /* KLOG_CONSTANTS_INCLUDED */
