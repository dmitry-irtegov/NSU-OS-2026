#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        perror("pipe creating failure");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(pipefd[0]);
        char string[] = "SomeThing";
        if (write(pipefd[1], string, strlen(string) + 1) == -1) {
            perror("write error");
            exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
    } else {
        close(pipefd[1]);
        char buf[256] = {0};
        ssize_t n = 0;
        ssize_t b;
        while (n < sizeof(buf) && (b = read(pipefd[0], buf + n, sizeof(buf) - n)) > 0) {
            n += b;
        }
        if (b == -1) {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
        for (int i = 0; i < strlen(buf); i++) {
            buf[i] = toupper((unsigned char)buf[i]);
        }
        printf("%s\n", buf);

        if (wait(NULL) == -1) {
            perror("wait failed");
            exit(1);
        }
    }
    return 0;
}
