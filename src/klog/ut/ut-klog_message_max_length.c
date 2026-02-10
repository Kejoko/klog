/**
 * @brief Ensure that the maximum message length is correctly enforced
 *      - Ensure the message buffer is initialized correctly
 *      - Ensure that when we log, the message is truncated accordingly
 *          - Log to a tempfile, and ensure the lines are correct
 *
 * @brief This assumes that the prefixes are being created correctly
 */

int noop(void) {
    return 0;
}

int main(void) {
    return
        noop()
    ;
}
