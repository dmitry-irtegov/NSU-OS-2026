#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/task31.sock"
#define BUFFER_SIZE 4096

struct sockaddr_un server_addr;
char buffer[BUFFER_SIZE];

int main(void) {
    signal(SIGPIPE, SIG_IGN);

    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        return 1;
    }

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    ssize_t bytes_read;
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof buffer)) > 0) {
        ssize_t written = 0;
        while (written < bytes_read) {
            ssize_t res = write(sock_fd, buffer + written, (size_t)(bytes_read - written));
            if (res == -1) {
                perror("write to socket");
                close(sock_fd);
                return 1;
            }
            written += res;
        }
    }

    if (bytes_read == -1) {
        perror("read from stdin");
        close(sock_fd);
        return 1;
    }

    close(sock_fd);
    return 0;
}
