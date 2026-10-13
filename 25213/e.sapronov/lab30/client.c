#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *socket_name = "cool.socket";
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un un;
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strncpy(un.sun_path, socket_name, sizeof(un.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr *)&un, sizeof(un)) == -1) {
        perror("connect");
        exit(1);
    }

    ssize_t readcnt;
    char buff[BUFSIZ];
    while ((readcnt = read(STDIN_FILENO, buff, sizeof(buff))) > 0) {
        ssize_t sent = 0;
        while (sent < readcnt) {
            ssize_t n = send(sockfd, buff + sent, readcnt - sent, 0);
            if (n == -1) {
                perror("send");
                exit(1);
            }
            if (n == 0) {
                fprintf(stderr, "send: no progress\n");
                exit(1);
            }
            sent += n;
        }
    }

    if (readcnt == -1) {
        perror("read");
        exit(1);
    }

    close(sockfd);
    return 0;
}
