#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "please pass command and arguments to run\n");
        exit(2);
    }

    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("failed to fork");
        exit(1);
    }
    else if (pid == 0) {
        // child
        execvp(argv[1], argv + 1);

        perror("failed to execvp");
        exit(1);
    }
    else if (pid > 0) {
        // parent
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            exit(1);
        }

        if (WIFEXITED(status) != 0) {
            fprintf(stdout, "exited, return code: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status) != 0) {
            fprintf(stdout, "process was signaled, code: %d\n", WTERMSIG(status));
        }
    }

    exit(0);
}
