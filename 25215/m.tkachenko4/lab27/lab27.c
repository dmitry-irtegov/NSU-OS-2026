
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    FILE *pipe_stream = popen("grep '^$' | wc -l", "w");
    if (pipe_stream == NULL) {
        perror("popen");
        fclose(file);
        return 1;
    }

    char buffer[SIZE];
    size_t n;
    int error = 0;

    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        size_t written = 0;

        while (written < n) {
            size_t k = fwrite(buffer + written, 1, n - written, pipe_stream);

            if (k == 0) {
                if (ferror(pipe_stream))
                    perror("fwrite");
                else
                    fprintf(stderr, "fwrite: no progress\n");

                error = 1;
                break;
            }

            written += k;
        }

        if (error)
            break;
    }

    if (ferror(file)) {
        perror("fread");
        error = 1;
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        error = 1;
    }

    int status = pclose(pipe_stream);

    if (status == -1) {
        perror("pclose");
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Pipeline failed\n");
        error = 1;
    }

    return error ? 1 : 0;
}
