#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(fd[1]);

        FILE *input = fdopen(fd[0], "r");
        if (input == NULL) {
            perror("fdopen input");
            close(fd[0]);
            return EXIT_FAILURE;
        }

        unsigned char buffer[256];
        size_t count;

        while ((count = fread(buffer, 1, sizeof(buffer), input)) > 0) {
            for (size_t i = 0; i < count; ++i) {
                buffer[i] = (unsigned char)toupper(buffer[i]);
            }

            if (fwrite(buffer, 1, count, stdout) != count) {
                perror("fwrite stdout");
                fclose(input);
                return EXIT_FAILURE;
            }
        }

        if (ferror(input)) {
            perror("fread");
            fclose(input);
            return EXIT_FAILURE;
        }

        fclose(input);

        if (fflush(stdout) == EOF) {
            perror("fflush stdout");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    close(fd[0]);
    int result = EXIT_SUCCESS;

    FILE *output = fdopen(fd[1], "w");
    if (output == NULL) {
        perror("fdopen output");
        close(fd[1]);
        result = EXIT_FAILURE;
    } else {
        const char text[] = "Hello, UNIX! This is lab 25.\n";
        size_t length = sizeof(text) - 1;

        if (fwrite(text, 1, length, output) != length) {
            perror("fwrite pipe");
            result = EXIT_FAILURE;
        }

        if (fclose(output) == EOF) {
            perror("fclose output");
            result = EXIT_FAILURE;
        }
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        return EXIT_FAILURE;
    }

    return result;
}