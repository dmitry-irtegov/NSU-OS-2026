#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *file_name = (argc > 1) ? argv[1] : "/etc/profile";

    printf("Parent started, PID: %ld\n", (long)getpid());
    fflush(stdout);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", file_name, (char *)NULL);
        perror("execlp");
        _exit(1);
    }

    printf("Child created with PID %ld, reading %s\n", (long)pid, file_name);
    fflush(stdout);

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }

    printf("Child process finished, parent exiting\n");
    return 0;
}
