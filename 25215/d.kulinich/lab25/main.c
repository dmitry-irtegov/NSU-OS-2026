#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(void) {
    int fd[2];  // fd[0] like stdin  - for read
                // fd[1] like stdout - for write

    if (pipe(fd) == -1) {
        perror("pipe doesn't created");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        ssize_t bytes_read;
        char buffer[BUFFER_SIZE];

        close(fd[1]);

        while ((bytes_read = read(fd[0], buffer, sizeof(buffer))) > 0) {
            ssize_t bytes_written = 0;

            for (ssize_t i = 0; i < bytes_read; i++) {
                buffer[i] = (char)toupper(buffer[i]);
            }

            while (bytes_written < bytes_read) {
                ssize_t res = write(STDOUT_FILENO, 
                                    buffer + bytes_written, 
                                    bytes_read - bytes_written);
                if (res == -1) {
                    perror("failed to write in STDOUT");
                    close(fd[0]);
                    exit(1);
                }
                bytes_written += res;
            }
        }

        if (bytes_read == -1) {
            perror("failed to read from pipe");
            close(fd[0]);
            exit(1);
        }

        close(fd[0]);
        exit(0);

    } else {

        const char *message = "Hello, UNIX SVR4! This is a MiXeD cAsE MeSsAgE.\n";
        size_t msg_len = strlen(message);
        ssize_t bytes_written = 0;

        close(fd[0]);

        while (bytes_written < (ssize_t)msg_len) {
            ssize_t res = write(fd[1], 
                                message + bytes_written, 
                                msg_len - bytes_written);
            if (res == -1) {
                perror("failed to write in pipe");
                close(fd[1]);
                wait(NULL);
                return 1;
            }
            bytes_written += res;
        }

        close(fd[1]);

        if (wait(NULL) == -1) {
            perror("wait failed");
            return 1;
        }
    }

    return 0;
}