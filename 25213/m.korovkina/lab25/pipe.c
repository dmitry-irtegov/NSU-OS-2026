#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#define BUFSIZE 4096

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "No file name\n");
        return 1;
    }
    char* filename = argv[1];
    int fildes[2];
    if (pipe(fildes) == -1) {
        perror("Failed to create a pipe");
        return 1;
    }
    char buff[BUFSIZE];
    pid_t pid = fork();
    if (pid > 0) {
        close(fildes[0]);
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open a file");
            return 1;
        }
        ssize_t byte_count;
        while ((byte_count = read(fd, buff, sizeof(buff))) > 0) {
            if (write(fildes[1], buff, byte_count) == -1) {
                perror("Parent write");
                return 1;
            }
        }
        if (byte_count == -1) {
            perror("Parent read");
            return 1;
        }
        close(fildes[1]);
    } else if (pid == 0) {
        close(fildes[1]);
        ssize_t byte_count;
        while ((byte_count = read(fildes[0], buff, sizeof(buff))) > 0) {
            for (ssize_t i = 0; i < byte_count; i++) {
                buff[i] = (char)toupper((unsigned char)buff[i]);
            }
            if (write(STDOUT_FILENO, buff, byte_count) == -1) {
                perror("Child write");
                return 1;
            }
        }
        if (byte_count == -1) {
            perror("Child read");
            return 1;
        }
        close(fildes[0]);
    } else {
        perror("Failed to create subprocess");
        return 1;
    }
    return 0;
}
