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
        perror("pipe creation error.");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork error.");
        return EXIT_FAILURE;
    }

    if (pid > 0) {

        close(pipefd[0]);

        char message[BUF_LIM];
        printf("[parent] type a message to be uppercased:\n");
        
        if (fgets(message, sizeof(message), stdin) != NULL) {
            size_t len = strlen(message);
            if (len > 0 && message[len - 1] == '\n') {
                message[len - 1] = '\0';
            }

            write(pipefd[1], message, strlen(message) + 1);
        }

        close(pipefd[1]);

    } else {

        close(pipefd[1]);

        char buffer[BUF_LIM];
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));

        if (bytes_read > 0) {
            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }

            printf("[child] uppercased text is:\n%s\n", buffer);
        }

        close(pipefd[0]);
    }

    return 0;
}
