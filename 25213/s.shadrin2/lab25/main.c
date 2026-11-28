#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>

#define BUF_SIZE 127

int main() {
    int fildes[2];
    if (pipe(fildes) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid != 0) {
        if (close(fildes[0]) == -1) {
            perror("close");
            return 1;
        }
        
        char sent_text[BUF_SIZE] = "Text To Be Uppered";
        if (write(fildes[1], sent_text, BUF_SIZE) == -1) {
            perror("write");
            return 1;
        }

    } else {
        close(fildes[1]);
        char received_text[BUF_SIZE];
        
        ssize_t n;
        while ((n = read(fildes[0], received_text, BUF_SIZE)) > 0) {
            for (int i = 0; i < n; i++) {
                received_text[i] = toupper(received_text[i]);
            }
            write(STDOUT_FILENO, received_text, n);
        }
        if (n == -1) {
            perror("read");
            return 1;
        }
        putchar('\n');
    }

    return 0;
}
