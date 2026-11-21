#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_PATH "unix_socket"

int main(void) {
    int socket_fd;
    int status = 1;
    struct sockaddr_un address = {0};
    char buffer[1024];
    ssize_t count;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return 1;
    }

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("connect");
        goto cleanup;
    }

    while ((count = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        ssize_t sent = 0;
        while (sent < count) {
            ssize_t written = write(socket_fd, buffer + sent, (size_t)(count - sent));

            if (written == -1) {
                perror("write");
                goto cleanup;
            }
            if (written == 0) {
                fprintf(stderr, "write: no data sent\n");
                goto cleanup;
            }
            sent += written;
        }
    }

    if (count == -1) {
        perror("read");
        goto cleanup;
    }
    status = 0;

cleanup:
    close(socket_fd);
    return status;
}