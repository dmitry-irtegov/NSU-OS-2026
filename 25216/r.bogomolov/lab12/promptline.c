#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

int prompline(char *line, int size) {
    if (isatty(STDIN_FILENO)) {
        printf("shell> ");
        fflush(stdout);
    }
    
    if (fgets(line, size, stdin) == NULL) {
        if (ferror(stdin)) {
            perror("fgets");
        }
        return 0;
    }
    if (strchr(line, '\n') == NULL) {
        int ch = getchar();

        if (ch != EOF) {
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }

            fprintf(stderr, "Слишком длинная строка\n");
            return -1;
        }
    }
    return 1;
}
