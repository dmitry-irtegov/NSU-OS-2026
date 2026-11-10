#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    int output_result;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s COMMAND [ARG ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror(argv[1]);
        _exit(EXIT_FAILURE);
    }

    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }

    if (WIFEXITED(status)) {
        output_result = printf("Exit code: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        output_result = printf("Terminated by signal: %d\n", WTERMSIG(status));
    } else {
        fprintf(stderr, "Unexpected child status.\n");
        return EXIT_FAILURE;
    }

    if (output_result < 0 || fflush(stdout) == EOF) {
        perror("stdout");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
