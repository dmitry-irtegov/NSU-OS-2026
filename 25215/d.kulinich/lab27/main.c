#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Using: %s <file_name>\n", argv[0]);
        return 1;
    }

    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe create failed");
        return 1;
    }

    pid_t pid_grep = fork();
    if (pid_grep == -1) {
        perror("fork for grep failed");
        return 1;
    }

    if (pid_grep == 0) {
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 in grep failded");
            exit(1);
        }

        close(fd[0]);
        close(fd[1]);

        // Regex "^$" means empty line.
        execlp("grep", "grep", "^$", argv[1], (char *)NULL);
        
        perror("grep doesn't launch");
        exit(1);
    }

    pid_t pid_wc = fork();
    if (pid_wc == -1) {
        perror("fork for wc failed");
        return 1;
    }

    if (pid_wc == 0) {
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2 in wc failed");
            exit(1);
        }

        close(fd[0]);
        close(fd[1]);

        execlp("wc", "wc", "-l", (char *)NULL);
        
        perror("wc doesn't launch");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    if (waitpid(pid_grep, NULL, 0) == -1) {
        perror("waitpid for grep failed");
    }
    if (waitpid(pid_wc, NULL, 0) == -1) {
        perror("waitpid for wc failed");
    }

    return 0;
}