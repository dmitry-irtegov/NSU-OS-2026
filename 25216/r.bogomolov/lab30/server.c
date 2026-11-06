#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_PATH "unix_socket"

int main(void) {
    int server_fd;
    int client_fd = -1;
    int status = 1;
    struct sockaddr_un address = {0};
    char buffer[1024];
    ssize_t count;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);
    
    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) == -1) {
        perror("listen");
        goto cleanup;
    }

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        goto cleanup;
    }

    while ((count = read(client_fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < count; ++i) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }

        if (fwrite(buffer, 1, (size_t)count, stdout) != (size_t)count || fflush(stdout) == EOF) {
            perror("stdout");
            goto cleanup;
        }
    }

    if (count == -1) {
        perror("read");
        goto cleanup;
    }
    status = 0;

cleanup:
    if (client_fd != -1) {
        close(client_fd);
    }
    close(server_fd);
    unlink(SOCKET_PATH);
    return status;
}
