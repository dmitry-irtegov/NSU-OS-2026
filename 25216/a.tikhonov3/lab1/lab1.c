#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    time_t now;
    struct tm *tm;
    char buf[64];

    if (setenv("TZ", "PST8", 1) != 0) {
        perror("setenv");
        return EXIT_FAILURE;
    }

    tzset();

    if (time(&now) == (time_t)-1) {
        perror("time");
        return EXIT_FAILURE;
    }

    tm = localtime(&now);
    if (tm == NULL) {
        perror("localtime");
        return EXIT_FAILURE;
    }

    if (strftime(buf, sizeof(buf),
                 "%Y-%m-%d %H:%M:%S %Z", tm) == 0) {
        fprintf(stderr, "strftime: буфер слишком мал\n");
        return EXIT_FAILURE;
    }

    printf("%s\n", buf);

    return EXIT_SUCCESS;
}