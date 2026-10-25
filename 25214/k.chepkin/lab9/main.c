#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char* args[] = {"sh", "-c", "cat file.txt", NULL};
    char* envp[] = {NULL};

    pid_t pid = fork();
    if (pid == 0) {
        int ret = execve("/bin/sh", args, envp);
        if (ret == -1) {
            perror("exec failed");
            exit(1);
        }
    }
    else if (pid == -1) {
        perror("fork call failed");
        exit(1);
    }
    else {
        siginfo_t info;
        int ret = waitid(P_PID, pid, &info, WEXITED);
        if (ret == -1) {
            perror("wait call failed");
            exit(1);
        }
        printf("\n\n\nsome text\n\n\n");
    }
    exit(0);
}