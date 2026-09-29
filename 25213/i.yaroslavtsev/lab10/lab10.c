#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [command] [args]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {

        perror("Child process creation error.\n");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {

        execv(argv[1], &argv[1]);

        perror("execv error.\n");
        exit(EXIT_FAILURE);

    } else {

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error waiting for a child process.\n");
        }

        if (WIFEXITED(status)) {
            printf("Child process finished. Exit code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process was terminated by signal: %d\n", WTERMSIG(status));
        }

    }

    return EXIT_SUCCESS;
}
