#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

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

    int status = pclose(fp);

    if (status == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        printf("tr finished with code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
        printf("tr terminated by signal: %d\n", WTERMSIG(status));
    }

    return 0;
}
