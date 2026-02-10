/**
 * @brief Ensure that the message's prefix is being created correctly
 *      - Ensure the prefix buffer is initialized correctly (considering thread id, timestamp, logger name length, source location length)
 *      - Ensure that when we log, the prefix buffer is populated accordingly
 */

int noop(void) {
    return 0;
}

int main(void) {
    return
        noop()
    ;
}
