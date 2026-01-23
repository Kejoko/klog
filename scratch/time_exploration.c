#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __linux__
#include <sys/time.h> /* For gettimeofday() */
#include <time.h>     /* For time(), ctime() */
#endif

/**
 * @brief We need to use a combination of gettimeofday() and localtime()
 *      The gettimeofday() function provides microseconds, while the
 *      locatime() function provides everything else.
 */

void test_a(void) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL)) {
        printf("Error when invoking gettimeofday()\n");
        exit(1);
    }

    printf("Time of day sec : %ld\n", tv.tv_sec);
    printf("Time of day usec: %ld\n", tv.tv_usec);
    printf("\n");
}

void print_time(void) {
    const time_t tt = time(NULL);

    char* c = ctime(&tt);
    int formatted_length = strlen(c) - 1; /* To get rid of the newline */

    printf("Time string: \"%s\"\n", c);
    printf("Time string formatted: \"%.*s\"\n", formatted_length, c);
    printf("\n");
}

void broken_down(void) {
    /* This works with either gmtime() or localtime() */
    const time_t tt = time(NULL);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* bdt = localtime(&tt);
    uint32_t actual_sec = tv.tv_sec % 60;
    
    printf("Year  %d\n", bdt->tm_year + 1900);
    printf("Month %d\n", bdt->tm_mon);
    printf("Day   %d\n", bdt->tm_mday);
    printf("\n");
    printf("Hour %d\n", bdt->tm_hour);
    printf("Min  %d\n", bdt->tm_min);
    printf("Sec  %d\n", bdt->tm_sec);
    printf("     %d\n", actual_sec);
    printf("USec %ld\n", tv.tv_usec);
}

void formatted(void) {
    /* This works with either gmtime() or localtime() */
    const time_t tt = time(NULL);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* bdt = localtime(&tt);
    uint32_t actual_sec = tv.tv_sec % 60;
    
    printf("Year  %.6d\n", bdt->tm_year + 1900);
    printf("Month %.6d\n", bdt->tm_mon);
    printf("Day   %.6d\n", bdt->tm_mday);
    printf("\n");
    printf("Hour %.3d\n", bdt->tm_hour);
    printf("Min  %.3d\n", bdt->tm_min);
    printf("Sec  %.3d\n", bdt->tm_sec);
    printf("     %.3d\n", actual_sec);
    printf("USec %.10ld\n", tv.tv_usec);
}

int main(void) {
    test_a();
    print_time();
    broken_down();
    formatted();
}
