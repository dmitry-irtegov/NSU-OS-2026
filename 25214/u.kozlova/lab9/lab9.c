#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execlp("cat", "cat", "lab9.txt", NULL);
        perror("execlp");
        exit(1);
    }

    printf("Родитель\n");
    waitpid(pid, NULL, 0);
    printf("Конец\n");

    return 0;
}