#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Укажи команду для %s\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execvp(argv[1], &argv[1]);

        perror(argv[1]);
        _exit(127);
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("Команда '%s' завершилась с кодом: %d\n", argv[1], exit_code);
    } else if (WIFSIGNALED(status)) {
        printf("Команда '%s' была убита сигналом №%d\n", argv[1], WTERMSIG(status));
    }

    return EXIT_SUCCESS;
}