#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buf[256];
    ssize_t n = 0;
    ssize_t b;
    while (n < sizeof(buf)) {
        b = read(STDIN_FILENO, buf + n, sizeof(buf) - n);
        if (b > 0) {
            n += b;
        } else if (b == 0) {
            break;
        } else if (errno == EINTR) {
            continue;
        } else {
            perror("read error");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < strlen(buf); i++) {
        buf[i] = toupper((unsigned char)buf[i]);
    }
    printf("%s\n", buf);
    return 0;
}
