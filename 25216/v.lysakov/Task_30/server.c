#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/task16.sock"

int main(void) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path),
             "%s", SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr,
             sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    char buffer[1024];
    ssize_t n;

    while ((n = read(client_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            putchar(toupper((unsigned char)buffer[i]));
        }
        fflush(stdout);
    }

    if (n == -1) {
        perror("read");
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return n == -1 ? 1 : 0;
}