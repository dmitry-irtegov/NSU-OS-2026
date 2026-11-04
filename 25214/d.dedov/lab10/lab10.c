#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("not enough args\n");
        return 0;
    }
    pid_t prog = fork();
    if (prog == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (prog == 0) {
        printf("child\n");
        execvp(argv[1], argv + 1);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    int status;
    if (wait(&status) == -1) {
        perror("wait failed");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        printf("%d\n", exit_status);
        return 0;
    }
    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        printf("term by sig: %d\n", sig);
        return 0;
    }
    return 0;
}
