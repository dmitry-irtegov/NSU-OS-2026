#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char *sock_path = "./socket";

int main () {
    int serv_fildes;
    int clnt_fildes;
    struct sockaddr_un addr;

    if ((serv_fildes = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    if (bind(serv_fildes, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    if (listen(serv_fildes, 1) == -1) {
        perror("listen failed");
        unlink(sock_path);
        return EXIT_FAILURE;
    }

    if ((clnt_fildes = accept(serv_fildes, NULL, NULL)) == -1) {
        perror("accept failed");
        unlink(sock_path);
        return EXIT_FAILURE;
    }

    ssize_t rd;
    char buffer[BUFSIZ];
    while ((rd = read(clnt_fildes, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < rd; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        ssize_t wr = 0;
        ssize_t wr_once;
        while (wr < rd) {
            if ((wr_once = write(STDOUT_FILENO, buffer + wr, rd - wr)) == -1) {
                perror("write failed");
                unlink(sock_path);
                return EXIT_FAILURE;
            }
            wr += wr_once;
        }
    }

    if (rd == -1) {
        perror("read failed");
        unlink(sock_path);
        return EXIT_FAILURE;
    }

    unlink(sock_path);
    return EXIT_SUCCESS;
}
