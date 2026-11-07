#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "/tmp/task30.sock"
#define BUFFER_SIZE 1024

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Cannot create socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    printf("Waiting for connect to %s...\n", SOCKET_PATH);

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    printf("Client connected!\n");

    ssize_t bytes_read;
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t res = write(STDOUT_FILENO, buffer + bytes_written, bytes_read - bytes_written);
            if (res == -1) {
                perror("failed to frite in STDOUT");
                break;
            }
            bytes_written += res;
        }
    }

    if (bytes_read == -1) {
        perror("Cannot read from socket");
    }

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}