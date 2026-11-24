
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock_fd, (struct sockaddr *)&addr,
                sizeof(addr)) == -1) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Присоединение к серверу установлено.\n");
    printf("Введите текст: ");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        size_t sent = 0;

        while (sent < len) {
            ssize_t n = write(sock_fd, buffer + sent, len - sent);

            if (n == -1) {
                perror("write");
                close(sock_fd);
                return EXIT_FAILURE;
            }

            sent += n;
        }
    }

    close(sock_fd);

    printf("\nКлиент: соединение закрыто.\n");

    return 0;
}
