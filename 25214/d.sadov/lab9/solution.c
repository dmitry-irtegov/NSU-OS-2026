#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    const char *fileName = "long_file.txt";

    printf("Parent: create process \n");
    fflush(stdout);

    pid_t pid = fork();

    if (pid == 0) {
        execl("/bin/cat", "cat", fileName, (char *)NULL);

        perror("execl failed");
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Parent: child process has been launched.\n");
        fflush(stdout);

        pid_t waited_pid = waitpid(pid, NULL, 0);

        if (waited_pid == -1) {
            perror("waitpid failed");
            exit(EXIT_FAILURE);
        }

        printf("\nParent: child process has finished.\n");
    }
    return 0;
}