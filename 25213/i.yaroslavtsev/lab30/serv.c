#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

const char *sock_path = "/tmp/socket";

int main () {
    int serv_fd;
    int conn_fd;
    struct sockaddr_un addr;

    if ((serv_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    if (bind(serv_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(serv_fd, 1) == -1) {
        perror("listen");
        unlink(sock_path);
        return EXIT_FAILURE;
    }

    if ((conn_fd = accept(serv_fd, NULL, NULL)) == -1) {
        perror("accept");
        unlink(sock_path);
        return EXIT_FAILURE;
    }
    unlink(sock_path);

    ssize_t rd;
    char buffer[BUFSIZ];
    while ((rd = read(conn_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < rd; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }
        ssize_t wr = 0;
        ssize_t wr_once;
        while (wr < rd) {
            wr_once = write(STDOUT_FILENO, buffer + wr, rd - wr);
            if (wr_once == -1) {
                perror("write");
                return EXIT_FAILURE;
            }
            wr += wr_once;
        }
    }

    if (rd == -1) {
        perror("read");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
