#define __EXTENSIONS__ 1
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

int main(void) {
    enum { NUMBER_COUNT = 100, NUMBERS_PER_LINE = 10 };
    FILE *streams[2];

    const char *command =
        "while read number && [ -n \"$number\" ]; do "
        "printf '%s\\n' \"$number\"; done | sort -n";

    if (p2open(command, streams) == -1) {
        perror("p2open");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUMBER_COUNT; ++i) {
        if (fprintf(streams[0], "%d\n", rand() % 100) < 0) {
            perror("fprintf");
            p2close(streams);
            return EXIT_FAILURE;
        }
    }

    if (fputc('\n', streams[0]) == EOF || fflush(streams[0]) == EOF) {
        perror("write to pipeline");
        p2close(streams);
        return EXIT_FAILURE;
    }

    int sorted[NUMBER_COUNT];
    for (int i = 0; i < NUMBER_COUNT; ++i) {
        if (fscanf(streams[1], "%d", &sorted[i]) != 1) {
            fprintf(stderr, "Could not read all sorted numbers\n");
            p2close(streams);
            return EXIT_FAILURE;
        }
    }

    int status = p2close(streams);
    if (status == -1) {
        perror("p2close");
        return EXIT_FAILURE;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "sort -n failed\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUMBER_COUNT; ++i) {
        char separator = (i + 1) % NUMBERS_PER_LINE == 0 ? '\n' : ' ';
        if (printf("%d%c", sorted[i], separator) < 0) {
            perror("printf");
            return EXIT_FAILURE;
        }
    }
    if (fflush(stdout) == EOF) {
        perror("fflush stdout");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}