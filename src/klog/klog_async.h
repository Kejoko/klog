#ifndef KLOG_ASYNC_INCLUDED
#define KLOG_ASYNC_INCLUDED

#include <stdbool.h>

/**
 * @brief The body of the consumer threads.
 * @details While we should not stop, consume something from the message queue
 *      using klog_async_consume().
 * @note This is used within klog_initialize() when creating
 *      our consumer threads.
 * @param p A pointer to the data we want to give to the
 *      thread body. This is currently unused.
 */
void* klog_async_thread_body(
    void* p
);

/**
 * @brief Consume a message from the message queue.
 * @details Wait for the semaphore to denote that we have a message in the queue.
 *      Then check if we should deinitialize, if yes then stop according to our
 *      strategy (discard unconsumed messages or not). Get the index of the message
 *      which we should consume, and copy eerything into the staging buffer so the
 *      producers can have space freed up again. Output the message from our staging
 *      buffer.
 * @note This is invoked within the klog_async_thread_body so consumer threads
 *      may continually consume messages, and within the klog_log() implementation
 *      if we are not in async mode.
 */
bool klog_async_consume(
    void
);

#endif /* KLOG_ASYNC_INCLUDED */
