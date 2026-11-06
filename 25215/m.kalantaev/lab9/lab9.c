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

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execlp("cat", "cat", "--", argv[1], (char *)NULL);
        perror("execlp");
        _exit(EXIT_FAILURE);
    }

    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            return EXIT_FAILURE;
        }
    }

    printf("Parent: child has finished.\n");

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
