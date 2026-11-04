#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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

    if (bind(sockfd, (struct sockaddr *)&un, sizeof(un)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 1) == -1) {
        perror("listen");
        unlink(socket_name);
        exit(1);
    }

    int client = accept(sockfd, NULL, NULL);
    if (client == -1) {
        perror("accept");
        unlink(socket_name);
        exit(1);
    }
    unlink(socket_name);

    ssize_t readcnt;
    char buff[BUFSIZ];
    while ((readcnt = recv(client, buff, sizeof(buff), 0)) > 0) {
        for (int i = 0; i < readcnt; i++) {
            buff[i] = toupper((unsigned char)buff[i]);
        }

        ssize_t written = 0;
        while (written < readcnt) {
            ssize_t n = write(STDOUT_FILENO, buff + written, readcnt - written);
            if (n == -1) {
                perror("write");
                exit(1);
            }
            if (n == 0) {
                fprintf(stderr, "write: no progress\n");
                exit(1);
            }
            written += n;
        }
    }

    if (readcnt == -1) {
        perror("recv");
        exit(1);
    }

    close(client);
    close(sockfd);
    return 0;
}
