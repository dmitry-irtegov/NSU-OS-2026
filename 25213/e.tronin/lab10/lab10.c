#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s command [arguments]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("new process failed");
        return 1;
    }
    else if (pid == 0) {
        execvp(argv[1], &argv[1]);

        perror("execvp error");
        exit(1);
    }
    else {
        int stat;

        if (waitpid(pid, &stat, 0) == -1) {
            perror("waitpid error");
            return 1;
        }

        if (WIFEXITED(stat)) {
            printf("exit code: %d\n", WEXITSTATUS(stat));
        }
        else if (WIFSIGNALED(stat)) {
            printf("process was signaled. signal number: %d\n", WTERMSIG(stat));
        }
    }

    return 0;
}

