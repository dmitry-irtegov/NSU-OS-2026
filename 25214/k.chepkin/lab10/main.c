#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "name of the command and its arguments not provided.");
        exit(1);
    }
    char** command_argv = argv + 1;
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[1], command_argv);
        perror("Exec failed");
        exit(1);
    }
    else if (pid == -1) {
        perror("Fork failed");
        exit(1);
    }
    else {
        int stat_loc;
        pid = waitpid(pid, &stat_loc, 0);
        if (pid == -1) {
            exit(1);
        }
        int ret = 0;
        if (WIFEXITED(stat_loc) > 0) {
            int ret = printf("Exit status: %d", WEXITSTATUS(stat_loc));
        }
        if (ret != 0) {
            perror("Failed to print");
            exit(1);
        }
    }
    exit(0);
}