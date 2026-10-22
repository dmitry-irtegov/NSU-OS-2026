#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(fd[0]);

        const char *text = "Text from child";
        if (write(fd[1], text, strlen(text)) == -1) {
            perror("write");
            return 1;
        }

        close(fd[1]);
        return 0;
    }

    close(fd[1]);

    char buf[128]; 
    ssize_t n; 
    while((n = read(fd[0], buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        
        for(int i = 0; i < n; i++) {
            buf[i] = toupper((unsigned char)buf[i]);
        }
        printf("%s", buf);
    }

    if (n == -1) {
        perror("read"); 
        return 1;
    }

    printf("\n");

    close(fd[0]);
    if (waitpid(pid, NULL, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    return 0;
}
