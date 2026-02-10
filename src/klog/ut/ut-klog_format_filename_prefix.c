/**
 * @brief Ensure that the file's prefix is created correctly
 *      - We want to be sanitizing the filename prefix to get rid of any bad characters
 *        or things which would cause formatting issues
 */

int noop(void) {
    return 0;
}

int main(void) {
    return
        noop()
    ;
}
