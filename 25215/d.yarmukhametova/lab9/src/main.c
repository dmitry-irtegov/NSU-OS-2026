#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {

        if (execlp("cat", "cat", argv[1], NULL) == -1) {
            perror("execlp");
            exit(1);
        }

    } else {

        printf("parent: waits while cat is reading file\n");

        if (wait(NULL) == -1) {
            perror("wait");
            exit(1);
        }

        printf("finish!\n");
    }

    return 0;
}
