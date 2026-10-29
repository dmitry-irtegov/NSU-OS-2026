#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "bigfile.txt";

    printf("[PARENT, PID=%d] Запускаю подпроцесс для чтения %s...\n",
        (int)getpid(), filename);
    fflush(stdout);

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

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return EXIT_FAILURE;
    }

    printf("\n****************************************************************\n");
    printf("[PARENT PID=%d] ПОТОМОК С PID=%d ЗАВЕРШИЛСЯ! КОНЕЦ РАБОТЫ\n",
           (int)getpid(), (int)pid);
    printf("****************************************************************\n");

    return EXIT_SUCCESS;
}