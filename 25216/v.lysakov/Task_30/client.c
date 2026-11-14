#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/task16.sock"

int main(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path),
             "%s", SOCKET_PATH);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(fd);
        return 1;
    }

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        size_t sent = 0;

        while (sent < len) {
            ssize_t n = write(fd, buffer + sent, len - sent);

            if (n <= 0) {
                perror("write");
                close(fd);
                return 1;
            }

            sent += n;
        }
    }

    close(fd);
    return 0;
}