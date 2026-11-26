#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    FILE *output = popen("wc -l", "w");
    if (output == NULL) {
        perror("popen");
        exit(1);
    }

    int at_line_start = 1;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            if (at_line_start && fputc('\n', output) == EOF) {
                perror("fputc");
                exit(1);
            }
            at_line_start = 1;
        } else {
            at_line_start = 0;
        }
    }

    if (ferror(file)) {
        perror("fgetc");
        exit(1);
    }

    fclose(file);
    if (pclose(output) == -1) {
        perror("pclose");
        exit(1);
    }

    exit(0);
}
