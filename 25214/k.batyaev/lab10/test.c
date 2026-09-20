#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No command specified\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error creating process");
        return 1;
    } 
    else if (pid == 0) {
        execvp(argv[1], &argv[1]);

        perror("Error executing command");
        exit(EXIT_FAILURE);
    } 
    else {
        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("Error waiting for child process");
            return 1;
        }

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("Command completed with code: %d\n", exit_code);
        }

        else if (WIFSIGNALED(status)) {
            int sig_num = WTERMSIG(status);
            printf("Command was terminated by signal: %d\n", sig_num);
        }
    }

    return 0;
}
