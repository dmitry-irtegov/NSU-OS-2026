#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
    pid_t child = fork();
    if (child == 0) {
        execlp("cat", "cat", "something.txt", NULL);
        perror("exec failed");
        exit(1);
    } else if (child < 0) {
        perror("fork failed");
        exit(1);
    }
    if (wait(NULL) == -1) {
        perror("wait failed");
        exit(1);
    }
    printf("The program completed successfully\n");
    return 0;
}
