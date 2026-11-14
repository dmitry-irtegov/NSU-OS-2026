#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>

char *sock_path = "./socket";

int main () {
    int clnt_fildes;
    struct sockaddr_un addr;

    if ((clnt_fildes = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path)-1);

    if ((connect(clnt_fildes, (struct sockaddr*)&addr, sizeof(addr))) == -1) {
        perror("connect failed");
        return EXIT_FAILURE;
    }

    ssize_t rd;
    char buffer[BUFSIZ];
    while ((rd = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        ssize_t wr = 0;
        ssize_t wr_once;
        while (wr < rd) {
            if ((wr_once = write(clnt_fildes, buffer + wr, rd - wr)) == -1) {
                perror("write failed");
                return EXIT_FAILURE;
            }
            wr += wr_once;
        }
    }

    if (rd == -1) {
        perror("read failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
