#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No command specified!\n");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(2);
    }
    if (pid == 0) {
        execv(argv[1], &argv[1]);
        perror("execv");
        _exit(3);
    }
    if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(4);
        }
        if (WIFEXITED(status)) {
            printf("Child's exit code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal: %d\n", WTERMSIG(status));
        }
    }
    exit(0);
}