#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        if (execvp(argv[1], &argv[1]) == -1) {
            perror("execvp");
            exit(1);
        }
    } else {

        int log;
        if (waitpid(pid, &log, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        if (WIFEXITED(log)) {
            int exit_code = WEXITSTATUS(log);
            printf("Child process finished with exit code: %d\n", exit_code);
        } else if (WIFSIGNALED(log)) {
            int term_sig = WTERMSIG(log);
            printf("Child process was terminated by signal: %d\n", term_sig);
        } else {
            printf("Child process finished succesfully.\n");
        }
    }

    return 0;
}
