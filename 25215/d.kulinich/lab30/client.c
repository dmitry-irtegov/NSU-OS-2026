#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/task30.sock"

int main(void) {
    int sock_fd;
    struct sockaddr_un server_addr;
    const char *message = "Hello, UNIX SVR4! This is a MiXeD cAsE MeSsAgE.\n";
    size_t msg_len = strlen(message);
    

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Cannot create socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Cannot connect to server");
        close(sock_fd);
        return 1;
    }

    printf("Succesfully connect to server!\n");

    ssize_t bytes_written = 0;
    while (bytes_written < (ssize_t)msg_len) {
        ssize_t res = write(sock_fd, message + bytes_written, msg_len - bytes_written);
        if (res == -1) {
            perror("Connot write to socket");
            close(sock_fd);
            return 1;
        }
        bytes_written += res;
    }

    close(sock_fd);

    return 0;
}