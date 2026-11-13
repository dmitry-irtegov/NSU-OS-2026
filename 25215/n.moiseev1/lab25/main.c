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

        unsigned char ch;
        ssize_t count;

        while ((count = read(fd[0], &ch, 1)) > 0) {
            ch = (unsigned char)toupper(ch);

            if (write(STDOUT_FILENO, &ch, 1) != 1) {
                perror("write stdout");
                close(fd[0]);
                return EXIT_FAILURE;
            }
        }

        if (count == -1) {
            perror("read");
            close(fd[0]);
            return EXIT_FAILURE;
        }

        close(fd[0]);
        return EXIT_SUCCESS;
    }

    close(fd[0]);

    const char text[] = "Hello, UNIX! This is lab 25.\n";
    int result = EXIT_SUCCESS;

    for (size_t i = 0; i < sizeof(text) - 1; ++i) {
        if (write(fd[1], &text[i], 1) != 1) {
            perror("write pipe");
            result = EXIT_FAILURE;
            break;
        }
    }

    close(fd[1]);

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
