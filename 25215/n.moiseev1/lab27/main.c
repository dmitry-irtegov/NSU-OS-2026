#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    FILE *output = popen("grep '^$' | wc -l", "w");
    if (output == NULL) {
        perror("popen");
        fclose(input);
        return EXIT_FAILURE;
    }

    char buffer[256];
    size_t count;
    int result = EXIT_SUCCESS;

    while ((count = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        if (fwrite(buffer, 1, count, output) != count) {
            perror("fwrite");
            result = EXIT_FAILURE;
            break;
        }
    }

    if (ferror(input)) {
        perror("fread");
        result = EXIT_FAILURE;
    }
    fclose(input);

    int status = pclose(output);
    if (status == -1) {
        perror("pclose");
        return EXIT_FAILURE;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Pipeline failed\n");
        return EXIT_FAILURE;
    }

    return result;
}