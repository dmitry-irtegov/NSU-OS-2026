#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("Parent process started\n");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        execlp("cat", "cat", "text.txt", NULL);
        perror("execlp");
        exit(1);
    } else {
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
            return 1;
        }

        printf("Child process finished\n");
    }

    return 0;
}