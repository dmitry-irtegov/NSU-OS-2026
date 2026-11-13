#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execlp("cat", "cat", argv[1], (char *)NULL);

        perror("execlp");
        return EXIT_FAILURE;
    }

    printf("Parent process is waiting\n");

    if (waitpid(pid, NULL, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    printf("\nChild process is finished\n");

    return EXIT_SUCCESS;
}