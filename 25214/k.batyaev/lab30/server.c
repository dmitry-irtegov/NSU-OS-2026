#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "lab30.socket"

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buf[BUFSIZ];
    ssize_t r;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    while ((r = read(client_fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < r; i++) {
            buf[i] = toupper((unsigned char)buf[i]);
        }
        
        ssize_t written = 0;
        while (written < r) {
            ssize_t w = write(STDOUT_FILENO, buf + written, r - written);
            if (w == -1) {
                perror("write");
                unlink(SOCKET_PATH);
                exit(EXIT_FAILURE);
            }
            if (w == 0){
                fprintf(stderr, "write 0\n");
                unlink(SOCKET_PATH);
                exit(EXIT_FAILURE);
            }
            written += w;
        }
    }

    if (r == -1) {
        perror("read");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
