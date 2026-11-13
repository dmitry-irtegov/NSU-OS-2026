#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("fopen");
        return 1;
    }

    FILE *output = popen("wc -l", "w");
    if (output == NULL) {
        perror("popen");
        if (fclose(input) == EOF) {
            perror("fclose");
        }
        return 1;
    }

    char buffer[BUFSIZ];
    int beginning = 1;
    int failed = 0;
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        for (size_t i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                if (beginning) {
                    if (fputc('\n', output) == EOF) {
                        perror("fputc");
                        failed = 1;
                        break;
                    }
                }
                beginning = 1;
            } else {
                beginning = 0;
            }
        }

        if (failed) {
            break;
        }
    }

    if (ferror(input)) {
        perror("fread");
        failed = 1;
    }

    if (fclose(input) == EOF) {
        perror("fclose");
        failed = 1;
    }

    if (pclose(output) == -1) {
        perror("pclose");
        failed = 1;
    } 

    return failed;
}
