
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr,
             sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Сервер: ожидание подключения...\n");

    client_fd = accept(server_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Сервер: клиент подключен.\n");

    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {

        for (ssize_t i = 0; i < bytes_read; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        fwrite(buffer, 1, bytes_read, stdout);
        fflush(stdout);
    }

    if (bytes_read == -1) {
        perror("read");
    }

    printf("\nСервер: клиент отключен.\n");

    close(client_fd);
    close(server_fd);

    unlink(SOCKET_PATH);

    return 0;
}
