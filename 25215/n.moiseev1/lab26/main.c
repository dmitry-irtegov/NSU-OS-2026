#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void) {
    FILE *input = popen("printf 'Hello, UNIX! This is lab 26.\\n'", "r");
    if (input == NULL) {
        perror("popen");
        return EXIT_FAILURE;
    }

    unsigned char buffer[256];
    size_t count;

    while ((count = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        for (size_t i = 0; i < count; ++i) {
            buffer[i] = (unsigned char)toupper(buffer[i]);
        }

        if (fwrite(buffer, 1, count, stdout) != count) {
            perror("fwrite stdout");
            pclose(input);
            return EXIT_FAILURE;
        }
    }

    if (ferror(input)) {
        perror("fread");
        pclose(input);
        return EXIT_FAILURE;
    }

    int status = pclose(input);
    if (status == -1) {
        perror("pclose");
        return EXIT_FAILURE;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        return EXIT_FAILURE;
    }

    if (fflush(stdout) == EOF) {
        perror("fflush stdout");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}