
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>

#define SIZE 1024

int main(void)
{
    FILE *pipe_stream;
    char buffer[SIZE];
    size_t n;
    int status;
    pipe_stream = popen("printf 'Hello woRLD!\\n'", "r");

    if (pipe_stream == NULL) {
        perror("popen");
        return 1;
    }

    while ((n = fread(buffer, 1, sizeof(buffer), pipe_stream)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        size_t written = 0;
        while (written < n) {
            size_t k = fwrite(buffer + written, 1, n - written, stdout);
            written += k;
            if (ferror(stdout)) {
                perror("fwrite");
                pclose(pipe_stream);
                return 1;
            }
            if (k == 0) {
                fprintf(stderr, "fwrite: no progress\n");
                pclose(pipe_stream);
                return 1;
            }
        }
    }

    if (ferror(pipe_stream)) {
        perror("fread");
        pclose(pipe_stream);
        return 1;
    }

    status = pclose(pipe_stream);

    if (status == -1) {
        perror("pclose");
        return 1;
    }

    if (fflush(stdout) == EOF) {
        perror("fflush");
        return 1;
    }

    return 0;
}
