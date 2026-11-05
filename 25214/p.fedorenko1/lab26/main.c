#include <stdio.h>
#include <sys/wait.h>

int main(void) {
    FILE *stream = popen("tr a-z A-Z", "w");

    if (stream == NULL) {
        perror("popen");
        return 1;
    }

    const char message[] = "Hello, WorLD\n";

    if (fputs(message, stream) == EOF) {
        perror("fputs");
        pclose(stream);
        return 1;
    }

    int status = pclose(stream);

    if (status == -1) {
        perror("pclose");
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "child failed\n");
        return 1;
    }

    return 0;
}
