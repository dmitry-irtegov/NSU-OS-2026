#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#define READ_N 10


int main() {
    printf("Enter the line: ");
    fflush(stdout);

    char *line = NULL;
    size_t cap = 0;

    int fd[2];

    if (pipe(fd) ==  -1) {
        close(fd[0]);
        close(fd[1]);
        perror("pipe() caused an error\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        close(fd[0]);
        perror("fork() caused an error\n");
        exit(EXIT_FAILURE);
    case 0:
        close(fd[0]);
        ssize_t n = getline(&line, &cap, stdin);
        if (n == -1) {
            perror("getline() caused an error\n");
            exit(EXIT_FAILURE);
        }
        if (write(fd[1], line, n) == -1) {
            perror("write() from child caused an error\n");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        exit(EXIT_SUCCESS);

    default:
        close(fd[1]);
        char buf[READ_N];
        ssize_t readChars;
        while ((readChars = read(fd[0], buf, READ_N)) > 0) {
            for (ssize_t i = 0; i < readChars; i++) {
                buf[i] = toupper(buf[i]);;
            }
            if (write(STDOUT_FILENO, buf, readChars) == -1) {
                perror("write() from parent caused an error\n");
                exit(EXIT_FAILURE);
            }
        }
        if (readChars == -1) {
            perror("read() caused an error\n");
            exit(EXIT_FAILURE);
        }
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
}