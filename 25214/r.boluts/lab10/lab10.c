#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No command specified!\n");
        exit(2);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        execv(argv[1], &argv[1]);
        perror("execv");
        exit(3);
    }
    if (pid > 0) {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Child's exit code: %d\n", WEXITSTATUS(status));
        }
    }
    exit(0);
}