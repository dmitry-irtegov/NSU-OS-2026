#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "bigfile.txt";

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execlp("cat", "cat", filename, (char *)NULL);
        perror("execlp cat failed");
        _exit(EXIT_FAILURE);
    }

    printf("\n******************************\n");
    printf("[PARENT PID=%d CHILD=%d]\n",
           (int)getpid(), (int)pid);
    printf("******************************\n\n");

    return EXIT_SUCCESS;
}