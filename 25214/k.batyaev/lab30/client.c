#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "lab30.socket"

int main(void) {
    int socket_fd;
    struct sockaddr_un addr;
    char buf[BUFSIZ];
    ssize_t r;

    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    while ((r = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        ssize_t written = 0;
        while (written < r) {
            ssize_t w = write(socket_fd, buf + written, r - written);
            if (w == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            if (w == 0){
                fprintf(stderr, "write 0\n");
                exit(EXIT_FAILURE);
            }
            written += w;
        }
    }

    if (r == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(socket_fd);

    return 0;
}
