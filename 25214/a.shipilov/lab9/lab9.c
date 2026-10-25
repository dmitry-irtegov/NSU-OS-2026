#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Недостаточно аргументов\n");
        return 1;
    }

    const char *filename = argv[1];
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка при вызове fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execlp("cat", "cat", filename, (char *)NULL);

        perror("Ошибка при вызове execlp");
        exit(EXIT_FAILURE);
    } else {
        printf("Создание потомка с PID = %ld\n", (long)pid);

        if (wait(NULL) == -1) {
            perror("Ошибка wait");
            exit(EXIT_FAILURE);
        }

        printf("Потомок завершил работу.\n");
    }

    return 0;
}