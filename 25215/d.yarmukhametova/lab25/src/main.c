#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(void) {
    const char *string = "Hello World";

    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char buf[BUF_SIZE];
        ssize_t n;

        if (close(pipefd[1]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        while ((n = read(pipefd[0], buf, sizeof buf)) > 0) {
            for (ssize_t i = 0; i < n; ++i)
                buf[i] = (char)toupper((unsigned char)buf[i]);

            ssize_t off = 0;
            while (off < n) {
                ssize_t w = write(STDOUT_FILENO, buf + off, (size_t)(n - off));
                if (w == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                off += w;
            }
        }

        if (n == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, "\n", 1) != 1)
            perror("write");

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        size_t len = strlen(string);
        size_t off = 0;

        if (close(pipefd[0]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        while (off < len) {
            ssize_t w = write(pipefd[1], string + off, len - off);
            if (w == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            off += (size_t)w;
        }

        if (close(pipefd[1]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}