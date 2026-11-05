#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        //child
        close(fd[1]);

        char buffer[128];
        ssize_t n;

        while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) {
            
            for (ssize_t i = 0; i < n; i++) {
                buffer[i] = (char)toupper((unsigned char)buffer[i]);
            }

            if (write(STDOUT_FILENO, buffer, (size_t)n) == -1) { 
                perror("write");
                return 1;
            }

        }

        if (n == -1) {
            perror("read");
            return 1;
        }

        if (close(fd[0]) == -1) {
            perror("close read end");
            return 1;
        }

        return 0;
    }

    //parant
    close(fd[0]);
    int exit_status_code = 0;

    const char message[] = "Hello, WorLD";
    size_t length = sizeof(message) - 1;
    
    if (write(fd[1], message, length) == -1) {
        perror("write pipe");
        exit_status_code = 1;
    } 
    
    if (close(fd[1]) == -1) {
        perror("close write end");
        exit_status_code = 1;
    }

    int status;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "child failed\n");
        return 1;
    }

    return exit_status_code;
}