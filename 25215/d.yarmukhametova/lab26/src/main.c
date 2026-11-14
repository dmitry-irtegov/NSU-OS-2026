#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *string = "Hello World\n";

    FILE* fp = popen("tr '[:lower:]' '[:upper:]'", "w");

    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    if (fprintf(fp, "%s", string) < 0) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }

    if (pclose(fp) == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }

    return 0;
}
