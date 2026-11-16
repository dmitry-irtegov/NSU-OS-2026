
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024

int main(void)
{
    int fd[2];
    pid_t p1, p2;
    const char text[] = "Hello woRLD!\n";
    char buffer[SIZE];
    ssize_t n;

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    p1 = fork();
    if (p1 == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (p1 == 0) {
        close(fd[0]);

        size_t len = strlen(text);
        size_t sent = 0;

        while (sent < len) {
            ssize_t k = write(fd[1], text + sent, len - sent);

            if (k <= 0) {
                perror("write");
                _exit(1);
            }

            sent += (size_t)k;
        }

        close(fd[1]);
        _exit(0);
    }

    p2 = fork();
    if (p2 == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        waitpid(p1, NULL, 0);
        return 1;
    }

    if (p2 == 0) {
        close(fd[1]);

        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) {

            for (ssize_t i = 0; i < n; i++) {
                buffer[i] = (char)toupper((unsigned char)buffer[i]);
            }

            ssize_t sent = 0;

            while (sent < n) {
                ssize_t k = write(STDOUT_FILENO,
                                  buffer + sent,
                                  (size_t)(n - sent));

                if (k <= 0) {
                    perror("write");
                    _exit(1);
                }

                sent += k;
            }
        }

        if (n == -1) {
            perror("read");
            _exit(1);
        }

        close(fd[0]);
        _exit(0);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);

    return 0;
}
