#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>

#define BUF_LIM 101

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe creation error.\n");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork error.\n");
        return EXIT_FAILURE;
    }
    if (pid > 0) {
        close(pipefd[0]);
        char wMessage[BUF_LIM];
        printf("[prnt]: type a message to be uppercased:\n");
        if (fgets(wMessage, sizeof(wMessage), stdin) != NULL) {
            size_t mslen = strlen(wMessage);
            if ((mslen > 0) && (wMessage[mslen - 1] == '\n')) {
                wMessage[--mslen] = '\0';
            }
            write(pipefd[1], &mslen, sizeof(size_t));
            write(pipefd[1], wMessage, mslen);
        }
        close(pipefd[1]);
    }
    else {
        close(pipefd[1]);
        size_t mslen;
        char rMessage[BUF_LIM];
        ssize_t bytes_read = read(pipefd[0], &mslen, sizeof(size_t));
        if (bytes_read > 0) {
            read(pipefd[0], rMessage, mslen);
            rMessage[mslen] = '\0';
            for (int i = 0; rMessage[i] != '\0'; i++) {
                rMessage[i] = (char)toupper((unsigned char)(rMessage[i]));
            }
            printf("[chld]: uppercased!\n%s\n", rMessage);
        }
        close(pipefd[0]);
    }
    return 0;
}
