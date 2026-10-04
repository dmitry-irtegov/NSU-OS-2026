#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        execl("/usr/bin/cat", "cat", "file.txt", NULL);
        perror("execl");
        exit(2);
    }
    if (pid > 0) {
        printf("Line 1 from parent!\n");
        printf("Line 2 from parent!\n");
        wait(NULL);
        printf("Line 3 (LAST) from parent!\n");
    }
    exit(0);
}