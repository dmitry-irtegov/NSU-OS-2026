#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX_MSG_SIZE 20
int main() {
    static char text[MAX_MSG_SIZE] = "Hello, lab25!";
    static char buffer[MAX_MSG_SIZE];
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        exit(2);
    }
    if (pid > 0) {
        close(fd[1]);
        ssize_t readed = 0;
        while (readed < MAX_MSG_SIZE) {
            ssize_t n = read(fd[0], buffer + readed, MAX_MSG_SIZE - readed);
            if (n == -1) {
                perror("read");
                close(fd[0]);
                exit(3);
            }
            if (n == 0) {
                fprintf(stderr, "Unexpected EOF\n");
                close(fd[0]);
                exit(4);
            }
            readed += n;
        }
        close(fd[0]);
        char* ptr = buffer;
        while (*ptr != '\0') {
            *ptr = toupper((unsigned char) *ptr);
            ptr++;
        }
        printf("%s", buffer);
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
            exit(5);
        }
    } else {
        close(fd[0]);
        if (write(fd[1], text, MAX_MSG_SIZE) == -1) {
            perror("write");
            close(fd[1]);
            _exit(6);
        }
        close(fd[1]);
        _exit(0);
    }
    return 0;
}