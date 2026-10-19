#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    char command[128];
    char line[1024];
    int total_lines = 0;
    int non_empty_lines = 0;
    int empty_lines = 0;

    snprintf(command, sizeof(command), "wc -l \"%s\"", filename);

    FILE *pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("popen");
        return 1;
    }

    if (fscanf(pipe, "%d", &total_lines) != 1) {
        fprintf(stderr, "error reading wc output\n");
        pclose(pipe);
        return 1;
    }
    pclose(pipe);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    int in_long_line = 0;

    while (fgets(line, sizeof(line), fp)) {
        int is_empty = 1;
        int has_newline = 0;

        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '\n') {
                has_newline = 1;
            }
            else {
                is_empty = 0;
            }
        }

        if (!in_long_line) {
            if (!is_empty) {
                non_empty_lines++;
            }
        }

        in_long_line = !has_newline;
    }

    if (ferror(fp)) {
        fprintf(stderr, "error reading file\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    empty_lines = total_lines - non_empty_lines;

    printf("number of empty lines: %d\n", empty_lines);

    return 0;
}
